#include "tools/nvs_cust.h"
#include "commmon.h"
#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static nvs_handle_t nvs_handler;

int32_t nvs_read_counter() {
  esp_err_t ret = ESP_OK;
  int32_t msg_counter = 0;
  ret = nvs_get_i32(nvs_handler, "counter", &msg_counter);
  if (ret != ESP_OK) {
    return -1;
  }

  ESP_LOGE(TAG, "SUCCESSFULLY READ COUNTER");

  return msg_counter;
}

esp_err_t nvs_write_counter(int32_t new_counter) {
  esp_err_t ret = ESP_OK;
  ret = nvs_set_i32(nvs_handler, "counter", new_counter);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error WRING MSG COUNTER!!!");
    return ret;
  }

  ESP_LOGE(TAG, "SUCCESSFULLY WROTE TO COUNTER");

  return ret;
}

struct CustStr nvs_read_key() {

  struct CustStr out;
  esp_err_t ret = ESP_OK;
  size_t key_size = 0;
  ret = nvs_get_str(nvs_handler, "secret_key", NULL, &key_size);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "COULD NOT READ KEY FROM SOURCE");
    out.pointer = "";
    out.len = 0;
    return out;
  }
  out.pointer = malloc(key_size);
  out.len = key_size;
  ret = nvs_get_str(nvs_handler, "secret_key", out.pointer, &key_size);

  ESP_LOGE(TAG, "SUCCESSFULLY READ SECRET KEY");

  return out;
}

esp_err_t nvs_erase_main_keys() {

  esp_err_t ret = ESP_OK;

  ret = nvs_erase_key(nvs_handler, "counter");
  if (ret != ESP_OK) {

    ESP_LOGE(TAG, "COULD NOT ERASE COUNTER");
    return ret;
  }
  nvs_erase_key(nvs_handler, "secret_key");

  if (ret != ESP_OK) {

    ESP_LOGE(TAG, "COULD NOT ERASE SECRET KEY");
    return ret;
  }

  ESP_LOGI(TAG, "ERASED COUNTER AND KEY");
  return ret;
}

esp_err_t nvs_init_cust() {

  esp_err_t ret = ESP_OK;
  ESP_LOGI(TAG, "\nOpening Non-Volatile Storage (NVS) handle...");
  ret = nvs_open("storage", NVS_READWRITE, &nvs_handler);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(ret));
    return ret;
  }
  return ret;
}

esp_err_t nvs_initialize_key() {
  esp_err_t ret = ESP_OK;

  size_t key_size = 0;
  ESP_LOGI(TAG, "\nReading KEY from NVS...");
  ret = nvs_get_str(nvs_handler, "secret_key", NULL, &key_size);
  switch (ret) {
  case ESP_OK:
    char *key = malloc(key_size);
    ret = nvs_get_str(nvs_handler, "secret_key", key, &key_size);
    ESP_LOGI(TAG, "Read key= %s", key);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    ESP_LOGW(TAG, "KEY is not initialized yet!");
    ESP_LOGI(TAG, "INITIALIZING SECRET KEY");
    ret = nvs_set_str(nvs_handler, "secret_key", SECRET_KEY);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Error WRITING SECRET KEY!!!");
    }
    break;
  default:
    ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(ret));
  }
  return ret;
}
esp_err_t nvs_initialize_counter() {
  esp_err_t ret = ESP_OK;

  int32_t msg_counter = 0;
  ESP_LOGI(TAG, "\nReading counter from NVS...");
  ret = nvs_get_i32(nvs_handler, "counter", &msg_counter);
  switch (ret) {
  case ESP_OK:
    ESP_LOGI(TAG, "Read counter = %d", msg_counter);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    ESP_LOGW(TAG, "COUNTER is not initialized yet!");
    ESP_LOGI(TAG, "INITIALIZING MSG COUNTER");
    ret = nvs_set_i32(nvs_handler, "counter", msg_counter);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Error WRITING MSG COUNTER!!!");
    }
    break;
  default:
    ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(ret));
  }
  return ret;
}
