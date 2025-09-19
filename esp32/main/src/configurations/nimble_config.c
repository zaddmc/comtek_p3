#include "configurations/nimble_config.h"
#include "commmon.h"
#include "gap/gap_init.h"
#include "gatt/register_cb.h"

void ble_store_config_init(void);
void on_stack_reset(int reason);
void on_stack_sync(void);
void nimble_host_config_init(void);
void nimble_host_task(void *param);

void on_stack_reset(int reason) {
  /* On reset, print reset reason to console */
  ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}

void on_stack_sync(void) {
  /* On stack sync, do advertising initialization */
  adv_init();
}

void nimble_host_config_init(void) {
  /* Set host callbacks */
  ble_hs_cfg.reset_cb = on_stack_reset;
  ble_hs_cfg.sync_cb = on_stack_sync;
  ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
  ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

  /* Store host configuration */
  ble_store_config_init();
}

void nimble_host_task(void *param) {
  /* Task entry log */
  ESP_LOGI(TAG, "nimble host task has been started!");

  /* This function won't return until nimble_port_stop() is executed */
  nimble_port_run();

  /* Clean up at exit */
  vTaskDelete(NULL);
}
