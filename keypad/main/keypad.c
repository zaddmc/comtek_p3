#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include <string.h>

const gpio_num_t xPins[] = {GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_10};
const int xPinsSize = 3;
const gpio_num_t yPins[] = {GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3};
const int yPinsSize = 4;
const char KEYPAD_VALS[] = "123456789*0#";
const char PASSWORD[] = "030303";

#define TAG "KEYPAD"

void password_check(char password[]) {
    if (strcmp(password, PASSWORD)) {
        gpio_set_level(GPIO_NUM_8, 1);
        gpio_set_level(GPIO_NUM_9, 0);
        ESP_LOGI(TAG, "sss");
    } else {
        gpio_set_level(GPIO_NUM_9, 1);
        gpio_set_level(GPIO_NUM_8, 0);
    }
}

void app_main(void) {
    for (int i = 0; i < xPinsSize; i++) {
        gpio_set_direction(xPins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(xPins[i], GPIO_PULLDOWN_ONLY);
    }
    for (int i = 0; i < yPinsSize; i++) {
        gpio_set_direction(yPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(yPins[i], 0);
    }
    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_9, GPIO_MODE_OUTPUT);
    char keypad_input[100] = {'\0'};
    int input_idx = 0;
    while (true) {
        for (int i = 0; i < yPinsSize; i++) {
            gpio_set_level(yPins[i], 1);
            for (int j = 0; j < xPinsSize; j++) {
                if (gpio_get_level(xPins[j])) {
                    char ch = KEYPAD_VALS[i * xPinsSize + j];
                    if (ch == '#' || ch == '*') {
                        if (ch == '#') {
                            ESP_LOGI(TAG, "Value %s Was pressed", keypad_input);
                            ESP_LOGI(TAG, "asdh %s", PASSWORD);
                            password_check(keypad_input);
                        }
                        while (input_idx) {
                            keypad_input[--input_idx] = '\0';
                        }
                    } else {
                        keypad_input[input_idx++] = ch;
                    }
                }
                // Wait for release
                while (gpio_get_level(xPins[j])) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }

            gpio_set_level(yPins[i], 0);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
