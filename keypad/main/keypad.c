
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"

#define KEY_PIN_X2 GPIO_NUM_19  // both keys wired here
#define KEY_PIN_X3 GPIO_NUM_18  // both keys wired here
#define KEY_PIN_X4 GPIO_NUM_10 // both keys wired here

#define KEY_PIN_Y1 GPIO_NUM_0
#define KEY_PIN_Y2 GPIO_NUM_1
#define KEY_PIN_Y3 GPIO_NUM_2
#define KEY_PIN_Y4 GPIO_NUM_3


void app_main(void)
{
    gpio_set_direction(KEY_PIN_X2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEY_PIN_X2, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(KEY_PIN_X3, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEY_PIN_X3, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(KEY_PIN_X4, GPIO_MODE_INPUT);
    gpio_set_pull_mode(KEY_PIN_X4, GPIO_PULLDOWN_ONLY);

    gpio_set_direction(KEY_PIN_Y1, GPIO_MODE_OUTPUT);
    gpio_set_level(KEY_PIN_Y1, 0);


    gpio_set_direction(KEY_PIN_Y2, GPIO_MODE_OUTPUT);
    gpio_set_level(KEY_PIN_Y2, 0);

    gpio_set_direction(KEY_PIN_Y3, GPIO_MODE_OUTPUT);
    gpio_set_level(KEY_PIN_Y3, 0);

    gpio_set_direction(KEY_PIN_Y4, GPIO_MODE_OUTPUT);
    gpio_set_level(KEY_PIN_Y4, 0);


    while (true)
    {
      gpio_set_level(KEY_PIN_Y1, 1);
        if (gpio_get_level(KEY_PIN_X2))
        {
            printf("Key 1 was pressed\n");
            while (gpio_get_level(KEY_PIN_X2)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X3))
        {
            printf("Key 2 was pressed\n");
            while (gpio_get_level(KEY_PIN_X3)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X4))
        {
            printf("Key 3 was pressed\n");
            while (gpio_get_level(KEY_PIN_X4)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
      gpio_set_level(KEY_PIN_Y1, 0);
      gpio_set_level(KEY_PIN_Y2, 1);
        if (gpio_get_level(KEY_PIN_X2))
        {
            printf("Key 4 was pressed\n");
            while (gpio_get_level(KEY_PIN_X2)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X3))
        {
            printf("Key 5 was pressed\n");
            while (gpio_get_level(KEY_PIN_X3)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X4))
        {
            printf("Key 6 was pressed\n");
            while (gpio_get_level(KEY_PIN_X4)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
      gpio_set_level(KEY_PIN_Y2, 0);
      gpio_set_level(KEY_PIN_Y3, 1);
        if (gpio_get_level(KEY_PIN_X2))
        {
            printf("Key 7 was pressed\n");
            while (gpio_get_level(KEY_PIN_X2)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X3))
        {
            printf("Key 8 was pressed\n");
            while (gpio_get_level(KEY_PIN_X3)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X4))
        {
            printf("Key 9 was pressed\n");
            while (gpio_get_level(KEY_PIN_X4)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
      gpio_set_level(KEY_PIN_Y3, 0);
      gpio_set_level(KEY_PIN_Y4, 1);
        if (gpio_get_level(KEY_PIN_X2))
        {
            printf("Key * was pressed\n");
            while (gpio_get_level(KEY_PIN_X2)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X3))
        {
            printf("Key 0 was pressed\n");
            while (gpio_get_level(KEY_PIN_X3)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        if (gpio_get_level(KEY_PIN_X4))
        {
            printf("Key # was pressed\n");
            while (gpio_get_level(KEY_PIN_X4)){
              vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
      gpio_set_level(KEY_PIN_Y4, 0);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
