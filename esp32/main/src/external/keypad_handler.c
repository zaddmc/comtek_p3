#include "commmon.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "nvs.h"
#include "nvs/nvs_custom.h"
#include "nvs_flash.h"
#include "soc/gpio_num.h"
#include <external/display_handler.h>
#include <external/keypad_handler.h>
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

int is_unlocked = 0;

char keypad_input[17] = {'\0'};
int input_idx = 0;

int password_check(char password[]) {
  ESP_LOGI(TAG, "CHECKING KEYPAD PASSWORD WTIH INPUT: %s", password);
  esp_err_t ret = ESP_OK;

  if (strcmp(password, KEYPAD_EXTRA_TICKETS_KEY) == 0) {
    ESP_LOGI(TAG, "EXTRA TICKETS PASSWORD RECEIVED");
    int32_t unlocks = nvs_read_i32_custom(NVS_KEYPAD_TICKETS_TAG);
    if (unlocks < 0) {
      return -1;
    }
    unlocks += 5;
    ret = nvs_write_i32_custom(NVS_KEYPAD_TICKETS_TAG, unlocks);
    if (ret != ESP_OK) {
      return -1;
    }
    return 2;

  } else if (strcmp(password,
                    nvs_read_string_custom(NVS_KEYPAD_CODE_TAG).pointer) == 0) {
    ESP_LOGI(TAG, "KEYPAD PASSWORD RECEIVED");
    int32_t unlocks = nvs_read_i32_custom(NVS_KEYPAD_TICKETS_TAG);
    if (unlocks <= 0) {
      ESP_LOGI(TAG, "Not enough ticks to unlock");
      return -1;
    }
    unlocks--;
    gpio_set_level(RED_LED, 0);
    gpio_set_level(GREEN_LED, 1);
    is_unlocked = 1;
    ret = nvs_write_i32_custom(NVS_KEYPAD_TICKETS_TAG, unlocks);
    if (ret != ESP_OK) {
      return -1;
    }
    return 1;
  } else {
    gpio_set_level(GREEN_LED, 0);
    gpio_set_level(RED_LED, 1);
    is_unlocked = 0;
    return 1;
  }
}

void keypad_main() {
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
