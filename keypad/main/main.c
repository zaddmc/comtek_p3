#include "esp_log.h"
#include "keypad.h"
#include "location.h"
void app_main(void) {
    ESP_LOGI("MAIN", "Starting location tracking");
    location_main();
    ESP_LOGI("MAIN", "Finished location tracking");

    ESP_LOGI("MAIN", "Starting Keypad");
    keypad_main();
    ESP_LOGI("MAIN", "Finished Keypad");
}
