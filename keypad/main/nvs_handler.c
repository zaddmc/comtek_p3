#include "esp_log.h"
#include "inttypes.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_handler.h"
#include <stdint.h>

static nvs_handle_t HANDLE;

void init_nvs_handle(void) {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = nvs_open("storage", NVS_READWRITE, &HANDLE);
    if (err != ESP_OK) {
        ESP_LOGE("MAIN_NVS", "Error (%s) opening NVS handle!",
                 esp_err_to_name(err));
    }
    return;
}

void save_int(const char *key, const int32_t value) {
    ESP_LOGI("NVS", "Writing %i to key %s", value, key);
    esp_err_t err = nvs_set_i32(HANDLE, key, value);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Failed to write Int! key=%s  value=%d", key, value);
    }
}

int fetch_int(const char *key) {
    int32_t value = 0;
    esp_err_t err = nvs_get_i32(HANDLE, key, &value);
    switch (err) {
    case ESP_OK:
        ESP_LOGI("NVS", "Read key %s = %i", key, value);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGW("NVS", "The NVS is not initialized yet!");
        break;
    default:
        ESP_LOGE("NVS", "Error (%s) reading!", esp_err_to_name(err));
    }
    return value;
}

void save_string(const char *key, const char *value) {
    ESP_LOGI("NVS", "Writing %s to key %s", value, key);
    esp_err_t err = nvs_set_str(HANDLE, key, value);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Failed to write string! key=%s  value=%d", key, value);
    }
}

char *fetch_string(const char *key) {
    char *value;
    size_t rsize = 0;
    esp_err_t err = nvs_get_str(HANDLE, key, NULL, &rsize);
    if (err == ESP_OK) {
        value = malloc(rsize);
        nvs_get_str(HANDLE, key, value, &rsize);
        ESP_LOGI("NVS", "Read key %s = %s", key, value);
    } else {
        value = "\0";
    }
    return value;
}

void rolling_save_u64(const char *key, const uint64_t value) {
    nvs_handle_t nvs_handle;

    if (nvs_open("rolling_code", NVS_READWRITE, &nvs_handle) == ESP_OK) {
        nvs_set_u64(nvs_handle, key, value);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
        ESP_LOGI("ROL_NVS", "Set key %s with %llu", key, value);
    } else {
        ESP_LOGW("ROL_NVS", "Failed to save");
    }
}
