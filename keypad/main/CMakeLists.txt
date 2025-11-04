idf_component_register(SRCS "main.c" "keypad.c" "location.c" "gap_events.c"
                    INCLUDE_DIRS "." 
                    REQUIRES bt nvs_flash esp_driver_gpio)
