#include "commmon.h"
#include "configurations/nimble_config.h"
#include "esp_log.h"
#include "gap/gap_init.h"
#include "gatt/init_services.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "tools/led.h"
#include "tools/nvs_cust.h"
#include <stdint.h>

#define ERASE_MODE 0

void app_main(void) {
  led_init();
  /* Local variables */
  int status_code = 0;
  esp_err_t ret = ESP_OK;

  /* NVS flash initialization */
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_LOGE(TAG, "FLASH FULL ERASING");
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED to INITIALIZE nvs flash, error code: %d ", ret);
    return;
  }
  ret = nvs_init_cust();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO INITIALIZE CUST NVS, error code: %d ", ret);
    return;
  }
  if (ERASE_MODE) {
    ret = nvs_erase_main_keys();
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "FAILED TO ERASE NVS, error code: %d ", ret);
      return;
    }
    return;
  }
  ret = nvs_initialize_counter();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO INITIALIZE COUNTET TO NVS, error code: %d ", ret);
    return;
  }

  ret = nvs_initialize_key();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED TO INITIALIZE KEY TO NVS, error code: %d ", ret);
    return;
  }

  /* NimBLE stack initialization */
  ret = nimble_port_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "FAILED to initialize nimble stack, error code: %d ", ret);
    return;
  }

  /* GAP service initialization */
  status_code = gap_init();
  if (status_code != 0) {
    ESP_LOGE(TAG, "FAILED to initialize GAP service, error code: %d",
             status_code);
    return;
  }

  status_code = gatt_svc_init();

  if (status_code != 0) {
    ESP_LOGE(TAG, "FAILED to initialize GATT service, error code: %d",
             status_code);
    return;
  }

  /* NimBLE host configuration initialization */
  nimble_host_config_init();

  /* Start NimBLE host task thread and return */
  xTaskCreate(nimble_host_task, "NimBLE Host", 4 * 1024, NULL, 5, NULL);
  return;
}
