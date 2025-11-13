#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "keypad.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/gpio_num.h"
#include "ssd1306.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define green_led GPIO_NUM_9
#define red_led GPIO_NUM_8

static SSD1306_t _display;

const gpio_num_t INPUT_PINS[] = {GPIO_NUM_14, GPIO_NUM_13, GPIO_NUM_12};
const int INPUT_PINS_SIZE = 3;
const gpio_num_t OUTPUT_PINS[] = {GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6,
                                  GPIO_NUM_7, green_led,  red_led};
const int OUTPUT_PINS_SIZE = 4;

const char KEYPAD_VALS[] = "123456789*0#";

const char PASSWORD[] = "1234";
int is_unlocked = 0;

#define TAG "KEYPAD"

void save_int(nvs_handle_t handle, const char *key, int32_t value) {
    ESP_LOGI("NVS", "Writing %i to key %s", value, key);
    esp_err_t err = nvs_set_i32(handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Failed to write counter!");
    }
}
void save_string(nvs_handle_t handle, const char *key, const char *value) {
    ESP_LOGI("NVS", "Writing %s to key %s", value, key);
    esp_err_t err = nvs_set_str(handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Failed to write counter!");
    }
}
int fetch_int(nvs_handle_t handle, const char *key) {
    int32_t value = 0;
    esp_err_t err = nvs_get_i32(handle, key, &value);
    switch (err) {
    case ESP_OK:
        ESP_LOGI("NVS", "Read key %s = %i", key, value);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGW("NVS", "The value is not initialized yet!");
        break;
    default:
        ESP_LOGE("NVS", "Error (%s) reading!", esp_err_to_name(err));
    }
    return value;
}
char *fetch_string(nvs_handle_t handle, const char *key) {
    char *value;
    size_t rsize = 0;
    esp_err_t err = nvs_get_str(handle, key, NULL, &rsize);
    if (err == ESP_OK) {
        value = malloc(rsize);
        nvs_get_str(handle, key, value, &rsize);
        ESP_LOGI("NVS", "Read key %s = %s", key, value);
    } else {
        value = "FAIL";
    }
    return value;
}

void password_check(char password[], nvs_handle_t handle) {
    if (strcmp(password, "123456") == 0) {
        int32_t unlocks = fetch_int(handle, "unlocks");
        ESP_LOGI(TAG, "Adding 5 ticks to counter");
        unlocks += 5;
        save_int(handle, "unlocks", unlocks);
        ssd1306_display_text(&_display, 0, "Added 5 unlocks.", 16, false);

    } else if (strcmp(password, fetch_string(handle, "keycode")) == 0) {
        int32_t unlocks = fetch_int(handle, "unlocks");
        if (unlocks <= 0) {
            ESP_LOGI(TAG, "Not enough ticks to unlock");
            return;
        }
        unlocks--;
        gpio_set_level(red_led, 0);
        gpio_set_level(green_led, 1);
        is_unlocked = 1;
        save_int(handle, "unlocks", unlocks);
        ssd1306_display_text(&_display, 0, "Unlocked le lock", 16, false);
    } else {
        gpio_set_level(green_led, 0);
        gpio_set_level(red_led, 1);
        is_unlocked = 0;
        ssd1306_display_text(&_display, 0, "Locked the lock.", 16, false);
    }
}

void keypad_main(nvs_handle_t handle, SSD1306_t *display) {
    _display = *display;
    for (int i = 0; i < INPUT_PINS_SIZE; i++) {
        gpio_set_direction(INPUT_PINS[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(INPUT_PINS[i], GPIO_PULLDOWN_ONLY);
    }
    // The plus 2 is to init the green and red led
    for (int i = 0; i < OUTPUT_PINS_SIZE + 2; i++) {
        gpio_set_direction(OUTPUT_PINS[i], GPIO_MODE_OUTPUT);
        gpio_set_level(OUTPUT_PINS[i], 0);
    }
    gpio_set_level(red_led, 1);

    save_string(handle, "keycode", "1234");

    // Store and read an integer value
    /* int32_t unlocks = 42; */
    /* save_int(my_handle, "unlocks", unlocks); */

    // Read back the value
    int32_t read_counter = fetch_int(handle, "unlocks");
    ESP_LOGI("NVS", "It worked with val=%i", read_counter);

    char keypad_input[100] = {'\0'};
    int input_idx = 0;
    while (true) {
        for (int i = 0; i < 4; i++) {
            gpio_set_level(OUTPUT_PINS[i], 1);
            for (int j = 0; j < INPUT_PINS_SIZE; j++) {
                if (gpio_get_level(INPUT_PINS[j])) {
                    char ch = KEYPAD_VALS[i * INPUT_PINS_SIZE + j];
                    if (ch == '#' || ch == '*') {
                        if (ch == '#') {
                            password_check(keypad_input, handle);
                        }
                        while (input_idx) {
                            keypad_input[--input_idx] = '\0';
                        }
                    } else {
                        keypad_input[input_idx++] = ch;
                    }
                }
                // Wait for release
                while (gpio_get_level(INPUT_PINS[j])) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
            gpio_set_level(OUTPUT_PINS[i], 0);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
#undef TAG
