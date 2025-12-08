#include "actuator.h"
#include "ble_handler.h"
#include "common.h"
#include "display_handler.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "hal/rtc_io_types.h"
#include "keypad.h"
#include "nvs_handler.h"
#include "rolling.h"
#include "soc/gpio_num.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

const gpio_num_t INPUT_PINS[] = {KEY_PIN_N3_X2, KEY_PIN_N6_X3, KEY_PIN_N7_X4};
const int INPUT_PINS_SIZE = 3;
const gpio_num_t OUTPUT_PINS[] = {KEY_PIN_N8_Y1, KEY_PIN_N1_Y2, KEY_PIN_N4_Y3,
                                  KEY_PIN_N5_Y4};
const int OUTPUT_PINS_SIZE = 4;

const char KEYPAD_VALS[] = "123456789*0#";

bool is_unlocked = false;

rolling_code_ctx_t hmac_ctx;

char keypad_input[17] = {'\0'};
int input_idx = 0;

#define TAG "KEYPAD"
// #define POWER_PIN GPIO_NUM_11

void change_hmac_ctx(int64_t new_counter) {
    hmac_ctx.counter = new_counter;
    hmac_ctx.last_valid_counter = new_counter;
    hmac_ctx.failed_attempts = 0;
}

int set_briefcase_state(bool new_state) {
    if (new_state) {
        int32_t unlocks = fetch_int(NVS_UNLOCKS);
        if (unlocks <= 0) {
            ESP_LOGI(TAG, "Not enough ticks to unlock");
            return -1;
        }
        unlocks--;
        is_unlocked = true;
        // husb238_toggle_power(true);
        // gpio_set_level(POWER_PIN, 0);
        save_int(NVS_UNLOCKS, unlocks);
        return 1;
    }
    is_unlocked = false;
    // gpio_set_level(POWER_PIN, 1);
    //  husb238_toggle_power(false);
    return 1;
}

int password_check(char password[]) {
    if (strcmp(password, "123456") == 0) {
        int32_t unlocks = fetch_int(NVS_UNLOCKS);
        ESP_LOGI(TAG, "Adding 5 ticks to counter");
        unlocks += 5;
        save_int(NVS_UNLOCKS, unlocks);
        return 2;
    }
    if (rolling_code_verify_digits_auto(&hmac_ctx, atoi(keypad_input))) {
        return set_briefcase_state(true);
    }
    set_briefcase_state(false);
    return 1;
}

void keypad_main() {
    for (int i = 0; i < INPUT_PINS_SIZE; i++) {
        rtc_gpio_deinit(INPUT_PINS[i]);
        gpio_set_direction(INPUT_PINS[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(INPUT_PINS[i], GPIO_PULLDOWN_ONLY);
    }
    // The plus 2 is to init the green and red led
    for (int i = 0; i < OUTPUT_PINS_SIZE; i++) {
        rtc_gpio_deinit(OUTPUT_PINS[i]);
        gpio_set_direction(OUTPUT_PINS[i], GPIO_MODE_OUTPUT);
        gpio_set_level(OUTPUT_PINS[i], 0);
    }
    /* gpio_set_direction(POWER_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(POWER_PIN, 1); */

    // Read back the value
    update_display(0);

    ESP_LOGI("HMAC_TEST", "Starting HMAC TEST");
    ESP_ERROR_CHECK(rolling_code_init(&hmac_ctx, "TX_001"));
    ESP_LOGI("HMAC_TEST", "The thing is open, printing key");
    rolling_key_print(hmac_ctx.key);
    ESP_LOGI("HMAC_TEST", "The counter is %llu", hmac_ctx.counter);
    ESP_LOGI("HMAC_TEST", "The last valid counter is %llu",
             hmac_ctx.last_valid_counter);

    // The amount of loops to go thru before sleeping
    // As of currently there is a task delay of 50 ms per loop
    const int loop_target = 600000; // 6000 is roughly 5 minutes

    for (int c = 0; c < loop_target; c++) {
        for (int i = 0; i < 4; i++) {
            gpio_set_level(OUTPUT_PINS[i], 1);
            for (int j = 0; j < INPUT_PINS_SIZE; j++) {
                if (gpio_get_level(INPUT_PINS[j])) {
                    char ch = KEYPAD_VALS[i * INPUT_PINS_SIZE + j];
                    if (ch == '#' || ch == '*') {
                        int rc = 0;
                        if (ch == '#') {
                            rc = password_check(keypad_input);
                        }
                        while (input_idx)
                            keypad_input[--input_idx] = '\0';
                        update_display(rc);
                    } else {
                        if (input_idx == 16)
                            input_idx--;
                        keypad_input[input_idx++] = ch;
                        update_display(1);
                    }
                    // Reset Loop counter
                    c = 0;
                }
                // Wait for release
                while (gpio_get_level(INPUT_PINS[j]))
                    vTaskDelay(pdMS_TO_TICKS(10));
            }
            gpio_set_level(OUTPUT_PINS[i], 0);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Go to sleep
    stop_advertisement();

    for (int i = 0; i < OUTPUT_PINS_SIZE; i++) {
        rtc_gpio_init(OUTPUT_PINS[i]);
        rtc_gpio_set_direction(OUTPUT_PINS[i], RTC_GPIO_MODE_OUTPUT_ONLY);
        rtc_gpio_set_level(OUTPUT_PINS[i], 1);
    }

    for (int i = 0; i < INPUT_PINS_SIZE; i++) {
        rtc_gpio_init(INPUT_PINS[i]);
        rtc_gpio_set_direction(INPUT_PINS[i], RTC_GPIO_MODE_INPUT_ONLY);
        rtc_gpio_pulldown_en(INPUT_PINS[i]);
    }

    const uint64_t btn_mask = (1ULL << INPUT_PINS[0]) |
                              (1ULL << INPUT_PINS[1]) | (1ULL << INPUT_PINS[2]);

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_enable_ext1_wakeup_io(btn_mask, ESP_EXT1_WAKEUP_ANY_HIGH);

    ESP_LOGI(TAG, "Going to sleep zzz");
    esp_deep_sleep_start();
}
#undef TAG
