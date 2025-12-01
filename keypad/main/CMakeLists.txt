idf_component_register(SRCS "main.c" "keypad.c" "ble_handler.c" "nvs_handler.c" "display_handler.c" "rolling.c"
                       INCLUDE_DIRS "." 
                       REQUIRES bt driver nvs_flash esp_driver_gpio ssd1306 mbedtls)
