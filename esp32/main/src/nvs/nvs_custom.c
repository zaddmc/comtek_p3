#include "commmon.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <nvs/nvs_custom.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static nvs_handle_t nvs_handler;
esp_err_t nvs_init_custom() {

  esp_err_t ret = ESP_OK;
  ESP_LOGI(TAG, "\nOpening Non-Volatile Storage (NVS) handle...");
  ret = nvs_open("storage", NVS_READWRITE, &nvs_handler);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(ret));
    return ret;
  }
  return ret;
}

int32_t nvs_read_i32_custom(char *key) {
  ESP_LOGI(TAG, "READING I32 FROM NVS KEY: %s", key);
  esp_err_t ret = ESP_OK;
  int32_t read_value = 0;
  ret = nvs_get_i32(nvs_handler, key, &read_value);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO READ I32 FROM NVS KEY: %s", key);
    return -1;
  }

  ESP_LOGI(TAG, "SUCCESSFULLY READ I32 FROM NVS KEY: %s", key);
  return read_value;
}
esp_err_t nvs_write_i32_custom(char *key, int32_t val) {
  ESP_LOGI(TAG, "WRITING I32 TO NVS KEY: %s", key);
  esp_err_t ret = ESP_OK;
  ret = nvs_set_i32(nvs_handler, key, val);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO WRITE I32 TO NVS KEY: %s", key);
    return ret;
  }

  ESP_LOGI(TAG, "SUCCESSFULLY WRITE I32 TO NVS KEY: %s", key);
  return ret;
}

struct CustStr nvs_read_string_custom(char *key) {
  ESP_LOGI(TAG, "READING STRING FROM NVS KEY: %s", key);
  struct CustStr out;
  esp_err_t ret = ESP_OK;
  size_t key_size = 0;
  ret = nvs_get_str(nvs_handler, key, NULL, &key_size);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO READ STRING FROM NVS KEY: %s", key);
    out.pointer = "";
    out.len = 0;
    return out;
  }
  out.pointer = malloc(key_size);
  out.len = key_size;
  ret = nvs_get_str(nvs_handler, key, out.pointer, &key_size);

  ESP_LOGI(TAG, "SUCCESSFULLY READ STRING FROM NVS KEY: %s", key);

  return out;
}

esp_err_t nvs_write_string_custom(char *key, char *val) {
  ESP_LOGI(TAG, "WRITING STRING TO NVS KEY: %s", key);
  esp_err_t ret = ESP_OK;
  ret = nvs_set_str(nvs_handler, key, val);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO WRITE STRING TO NVS KEY: %s", key);
    return ret;
  }

  ESP_LOGI(TAG, "SUCCESSFULLY WROTE STRING TO NVS KEY: %s", key);

  return ret;
}

esp_err_t nvs_erase_key_custom(char *key) {
  ESP_LOGI(TAG, "ERASING NVS KEY: %s", key);
  esp_err_t ret = nvs_erase_key(nvs_handler, key);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO ERASE NVS KEY: %s", key);
    return ret;
  }
  ESP_LOGI(TAG, "SUCCESSFULLY ERASED NVS KEY: %s", key);
  return ret;
}

esp_err_t nvs_initialize_string_key(char *key, char *val) {
  ESP_LOGI(TAG, "INITIALIZING STRING NVS KEY: %s", key);
  esp_err_t ret = ESP_OK;
  struct CustStr out = nvs_read_string_custom(key);
  if (!out.pointer) {
    ret = ESP_ERR_NVS_NOT_FOUND;
  }
  switch (ret) {
  case ESP_OK:
    ESP_LOGI(TAG, "%s IS ALREADY INITIALIZED", key);
    ret = nvs_write_string_custom(key, val);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    ESP_LOGW(TAG, "%s IS NOT INITIALIZED", key);
    ESP_LOGI(TAG, "INITIALIZING %s", key);
    ret = nvs_write_string_custom(key, val);
    break;
  default:
    ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(ret));
  }
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "ERROR INITIALIZING STRING NVS KEY: %s", key);
  }
  return ret;
}

esp_err_t nvs_initialize_i32_key(char *key, int32_t val) {
  ESP_LOGI(TAG, "INITIALIZING I32 NVS KEY: %s", key);
  esp_err_t ret = ESP_OK;
  ret = nvs_read_i32_custom(key);
  switch (ret) {
  case ESP_OK:
    ESP_LOGI(TAG, "%s IS ALREADY INITIALIZED", key);
    ret = nvs_write_i32_custom(key, val);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    ESP_LOGW(TAG, "%s IS NOT INITIALIZED", key);
    ESP_LOGI(TAG, "INITIALIZING %s", key);
    ret = nvs_write_i32_custom(key, val);
    break;
  case -1:
    ESP_LOGW(TAG, "%s IS NOT INITIALIZED", key);
    ESP_LOGI(TAG, "INITIALIZING %s", key);
    ret = nvs_write_i32_custom(key, val);
    break;
  default:
    ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(ret));
  }
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "ERROR INITIALIZING I32 NVS KEY: %s", key);
  }
  return ret;
}

esp_err_t nvs_initialize_custom_keys() {
  esp_err_t ret = ESP_OK;
  ret = nvs_initialize_i32_key(NVS_ROLLING_CODE_COUNTER_TAG, 0);
  if (ret != ESP_OK) {
    return ret;
  }
  ret = nvs_initialize_string_key(NVS_ROLLING_CODE_KEY_TAG, SECRET_KEY);
  if (ret != ESP_OK) {
    return ret;
  }
  ret = nvs_initialize_string_key(NVS_KEYPAD_CODE_TAG, KEYPAD_KEY);
  if (ret != ESP_OK) {
    return ret;
  }
  ret = nvs_initialize_i32_key(NVS_KEYPAD_TICKETS_TAG, 0);
  if (ret != ESP_OK) {
    return ret;
  }
  return ret;
}
esp_err_t nvs_erase_custom_keys() {
  esp_err_t ret = ESP_OK;

  ret = nvs_erase_key_custom(NVS_ROLLING_CODE_COUNTER_TAG);
  if (ret != ESP_OK) {
    return ret;
  }
  ret = nvs_erase_key_custom(NVS_ROLLING_CODE_KEY_TAG);
  if (ret != ESP_OK) {
    return ret;
  }

  ret = nvs_erase_key_custom(NVS_KEYPAD_CODE_TAG);
  if (ret != ESP_OK) {
    return ret;
  }

  ret = nvs_erase_key_custom(NVS_KEYPAD_TICKETS_TAG);
  if (ret != ESP_OK) {
    return ret;
  }

  return ret;
}
