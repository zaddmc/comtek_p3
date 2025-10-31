#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include <string.h>

#define green_led GPIO_NUM_9
#define red_led GPIO_NUM_8

const gpio_num_t INPUT_PINS[] = {GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_10};
const int INPUT_PINS_SIZE = 3;
const gpio_num_t OUTPUT_PINS[] = {GPIO_NUM_0, GPIO_NUM_1, GPIO_NUM_2,
                                  GPIO_NUM_3, green_led,  red_led};
const int OUTPUT_PINS_SIZE = 4;

const char KEYPAD_VALS[] = "123456789*0#";

const char PASSWORD[] = "1234";
int is_unlocked = 0;

#define TAG "KEYPAD"

void password_check(char password[]) {
    if (strcmp(password, PASSWORD)) {
        gpio_set_level(green_led, 0);
        gpio_set_level(red_led, 1);
        is_unlocked = 0;
    } else {
        gpio_set_level(red_led, 0);
        gpio_set_level(green_led, 1);
        is_unlocked = 1;
    }
}

void app_main(void) {
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
                while (gpio_get_level(INPUT_PINS[j])) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
            gpio_set_level(OUTPUT_PINS[i], 0);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
