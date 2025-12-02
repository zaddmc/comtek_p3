file(GLOB_RECURSE srcs "main.c" "src/gap/*.c" "src/configurations/*.c" "src/gatt/*.c" "src/tools/*.c"  "src/nvs/*.c" "src/external/*.c")
idf_component_register(SRCS "${srcs}"
                       PRIV_REQUIRES bt nvs_flash esp_driver_gpio driver ssd1306
                       INCLUDE_DIRS "./include" )

