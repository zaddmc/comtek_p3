#include "esp_log.h"
#include "keypad.h"
#include "location.h"
#include "nvs.h"
#include "nvs_flash.h"

nvs_handle_t init_nvs_handle(void) {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS handle
    nvs_handle_t handle;
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE("MAIN_NVS", "Error (%s) opening NVS handle!",
                 esp_err_to_name(err));
    }
    return handle;
}

void app_main(void) {
    ESP_LOGI("MAIN", "Initializing NVS");
    nvs_handle_t handle = init_nvs_handle();

    ESP_LOGI("MAIN", "Starting location tracking");
    location_main(handle);

    ESP_LOGI("MAIN", "Starting Keypad");
    keypad_main(handle);
}
