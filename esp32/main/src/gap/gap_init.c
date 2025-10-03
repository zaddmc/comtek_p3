#include "gap/gap_init.h"
#include "commmon.h"
#include "gap/gap_event_handler.h"
#include "gap/shared_gap.h"

int set_fields_params(struct ble_hs_adv_fields *adv_fields,
                      struct ble_hs_adv_fields *rsp_fields,
                      struct ble_gap_adv_params *adv_params);
uint8_t own_addr_type;
uint8_t addr_val[6] = {0};
uint8_t esp_uri[16] = {BLE_GAP_URI_PREFIX_HTTPS,
                       '/',
                       '/',
                       'e',
                       's',
                       'p',
                       'r',
                       'e',
                       's',
                       's',
                       'i',
                       'f',
                       '.',
                       'c',
                       'o',
                       'm'};

inline static void format_addr(char *addr_str, uint8_t addr[]) {
  sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2],
          addr[3], addr[4], addr[5]);
}

void adv_init(void) {
  int rc = 0;
  char addr_str[18] = {0};

  /* Make sure we have proper BT identity address set */
  rc = ble_hs_util_ensure_addr(0);
  if (rc != 0) {
    ESP_LOGE(TAG, "device does not have any available bt address!");
    return;
  }

  /* Figure out BT address to use while advertising */
  rc = ble_hs_id_infer_auto(0, &own_addr_type);
  if (rc != 0) {
    ESP_LOGE(TAG, "failed to infer address type, error code: %d", rc);
    return;
  }

  /* Copy device address to addr_val */
  rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
  if (rc != 0) {
    ESP_LOGE(TAG, "failed to copy device address, error code: %d", rc);
    return;
  }
  format_addr(addr_str, addr_val);
  ESP_LOGI(TAG, "device address: %s", addr_str);

  /* Start advertising. */
  start_advertising();
}

int gap_init(void) {
  /* Local variables */
  int status_code = 0;

  /* Initialize GAP service */
  ble_svc_gap_init();

  /* Set GAP device name */
  status_code = ble_svc_gap_device_name_set(DEVICE_NAME);
  if (status_code != 0) {
    ESP_LOGE(TAG, "Failed to set device name to %s, error code: %d",
             DEVICE_NAME, status_code);
    return status_code;
  }

  /* Set GAP device appearance */
  status_code =
      ble_svc_gap_device_appearance_set(BLE_GAP_APPEARANCE_GENERIC_TAG);
  if (status_code != 0) {
    ESP_LOGE(TAG, "Failed to set device appearance, error code: %d",
             status_code);
    return status_code;
  }
  return status_code;
}
