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

#define GREEN_LED GPIO_NUM_9
#define RED_LED GPIO_NUM_8

// Naming convention for pins, Nx, where x is the pin number from left to right
#define PIN_N1_Y2 GPIO_NUM_4
#define PIN_N3_X2 GPIO_NUM_5
#define PIN_N4_Y3 GPIO_NUM_6
#define PIN_N5_Y4 GPIO_NUM_7
#define PIN_N6_X3 GPIO_NUM_12
#define PIN_N7_X4 GPIO_NUM_13
#define PIN_N8_Y1 GPIO_NUM_14
const gpio_num_t INPUT_PINS[] = {PIN_N3_X2, PIN_N6_X3, PIN_N7_X4};
const int INPUT_PINS_SIZE = 3;
const gpio_num_t OUTPUT_PINS[] = {PIN_N8_Y1, PIN_N1_Y2, PIN_N4_Y3,
                                  PIN_N5_Y4, GREEN_LED, RED_LED};
const int OUTPUT_PINS_SIZE = 4;

const char KEYPAD_VALS[] = "123456789*0#";

const char PASSWORD[] = "1234";
int is_unlocked = 0;

char keypad_input[17] = {'\0'};
int input_idx = 0;

#define TAG "KEYPAD"
static SSD1306_t _display;

void update_display(int state) {
    ssd1306_clear_screen(&_display, false);
    switch (state) {
    case 0:
        ssd1306_display_text(&_display, 0, "I'am Thorkild!", 14, false);
        ssd1306_display_text(&_display, 1, "Enter code to,", 14, false);
        ssd1306_display_text(&_display, 2, "Figure It Out", 13, false);
        ssd1306_display_text(&_display, 3, "with me\\ (^_^) /", 16, false);
        break;
    case 1:
        if (is_unlocked)
            ssd1306_display_text(&_display, 0, "Unlocked", 8, false);
        else
            ssd1306_display_text(&_display, 0, "Locked", 6, false);
        ssd1306_display_text(&_display, 1, "Current code", 12, false);
        ssd1306_display_text(&_display, 2, keypad_input, strlen(keypad_input),
                             false);
        ssd1306_display_text(&_display, 3, "   \\ (^_^) /", 13, false);
        break;
    case -1:
        ssd1306_display_text(&_display, 0, "Sorry", 5, false);
        ssd1306_display_text(&_display, 1, "No more ticks", 13, false);
        ssd1306_display_text(&_display, 2, "Contact renters", 15, false);
        ssd1306_display_text(&_display, 3, "Place for code", 14, false);
        break;
    case 2:
        ssd1306_display_text(&_display, 0, "THIS DEBUG ONLY!", 16, false);
        ssd1306_display_text(&_display, 1, "Added 5 ticks", 13, false);
        ssd1306_display_text(&_display, 3, "REMOVE THIS CODE", 16, false);
        break;
    default:
        ssd1306_display_text(&_display, 0, "Invalid state   ", 16, false);
        break;
    }
}

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
        value = "1234";
        nvs_set_str(handle, key, value);
        ESP_LOGW("NVS", "Failed to read NVS key: %s, which is now: %s", key,
                 value);
    }
    return value;
}

int password_check(char password[], nvs_handle_t handle) {
    if (strcmp(password, "123456") == 0) {
        int32_t unlocks = fetch_int(handle, "unlocks");
        ESP_LOGI(TAG, "Adding 5 ticks to counter");
        unlocks += 5;
        save_int(handle, "unlocks", unlocks);
        return 2;

    } else if (strcmp(password, fetch_string(handle, "keycode")) == 0) {
        int32_t unlocks = fetch_int(handle, "unlocks");
        if (unlocks <= 0) {
            ESP_LOGI(TAG, "Not enough ticks to unlock");
            return -1;
        }
        unlocks--;
        gpio_set_level(RED_LED, 0);
        gpio_set_level(GREEN_LED, 1);
        is_unlocked = 1;
        save_int(handle, "unlocks", unlocks);
        return 1;
    } else {
        gpio_set_level(GREEN_LED, 0);
        gpio_set_level(RED_LED, 1);
        is_unlocked = 0;
        return 1;
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
    gpio_set_level(RED_LED, 1);

    // Read back the value
    update_display(0);

    while (true) {
        for (int i = 0; i < 4; i++) {
            gpio_set_level(OUTPUT_PINS[i], 1);
            for (int j = 0; j < INPUT_PINS_SIZE; j++) {
                if (gpio_get_level(INPUT_PINS[j])) {
                    char ch = KEYPAD_VALS[i * INPUT_PINS_SIZE + j];
                    if (ch == '#' || ch == '*') {
                        int rc = 0;
                        if (ch == '#') {
                            rc = password_check(keypad_input, handle);
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
                }
                // Wait for release
                while (gpio_get_level(INPUT_PINS[j]))
                    vTaskDelay(pdMS_TO_TICKS(10));
            }
            gpio_set_level(OUTPUT_PINS[i], 0);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
#undef TAG
