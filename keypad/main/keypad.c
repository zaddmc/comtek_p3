#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"

const gpio_num_t xPins[] = {GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_10};
const int xPinsSize = 3;
const gpio_num_t yPins[] = {GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3};
const int yPinsSize = 4;
const char KEYPAD_VALS[] = "123456789*0#";

#define TAG "KEYPAD"

void app_main(void) {
    for (int i = 0; i < xPinsSize; i++) {
        gpio_set_direction(xPins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(xPins[i], GPIO_PULLDOWN_ONLY);
    }
    for (int i = 0; i < yPinsSize; i++) {
        gpio_set_direction(yPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(yPins[i], 0);
    }

    while (true) {
        for (int i = 0; i < yPinsSize; i++) {
            gpio_set_level(yPins[i], 1);
            for (int j = 0; j < xPinsSize; j++) {
                if (gpio_get_level(xPins[j])) {
                    ESP_LOGI(TAG, "Value %c Was pressed",
                             KEYPAD_VALS[i * xPinsSize + j]);
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
