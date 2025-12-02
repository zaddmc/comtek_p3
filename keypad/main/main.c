#include "ble_handler.h"
#include "display_handler.h"
#include "esp_log.h"
#include "keypad.h"
#include "nvs_handler.h"

void app_main(void) {
    ESP_LOGI("MAIN", "Initializing NVS");
    init_nvs_handle();

    save_string("google_find", "de830a881ae49b8347758540c6008a0373041556");
    // save_string("keycode", "12345");

    ESP_LOGI("MAIN", "Initializing SSD1306 Display");
    init_display_device();

    ESP_LOGI("MAIN", "STARTING DUAL ADVERTISEMENT");
    start_dual_advertising();

    ESP_LOGI("MAIN", "Starting Keypad");
    keypad_main();
}
