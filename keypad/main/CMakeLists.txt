idf_component_register(SRCS "main.c" "keypad.c" "location.c" "gap_events.c"
                    INCLUDE_DIRS "." 
                    REQUIRES bt driver nvs_flash esp_driver_gpio ssd1306)
