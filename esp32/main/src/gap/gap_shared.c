#include "gap/gap_event_handler.h"
#include "gap/shared_gap.h"
#include "host/ble_hs_adv.h"
#include <stdint.h>
#include <string.h>
const char *eid_string = "5589656432df127befd5bdceee51e89f39bdad8f";
uint8_t adv_raw_data[31] = {
    0x02, // Length
    0x01, // Flags data type value
    0x06, // Flags data
    0x19, // Length
    0x16, // Service data data type value
    0xAA, // 16-bit service UUID
    0xFE, // 16-bit service UUID
    0x41, // FMDN frame type with unwanted tracking protection mode indication
          // 20-byte ephemeral identifier (inserted below)
          // Hashed flags (implicitly initialized to 0)
};
void hex_string_to_bytes(const char *hex, uint8_t *bytes, size_t len) {
  for (size_t i = 0; i < len; i++) {
    sscanf(hex + 2 * i, "%2hhx", &bytes[i]);
  }
}

int set_fields_params(struct ble_hs_adv_fields *adv_fields,
                      struct ble_hs_adv_fields *rsp_fields,
                      struct ble_gap_adv_params *adv_params);

void start_advertising(void) {
  // 20-byte ephemeral identifier
  uint8_t eid_bytes[20];
  hex_string_to_bytes(eid_string, eid_bytes, 20);
  memcpy(&adv_raw_data[8], eid_bytes, 20);
  int status_code = 0;
  struct ble_hs_adv_fields adv_fields = {0};
  struct ble_hs_adv_fields rsp_fields = {0};
  struct ble_gap_adv_params adv_params = {0};

  status_code = set_fields_params(&adv_fields, &rsp_fields, &adv_params);

  ble_gap_adv_set_data(adv_raw_data, sizeof(adv_raw_data));

  status_code = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                                  &adv_params, gap_event_handler, NULL);
  if (status_code != 0) {
    ESP_LOGE(TAG, "failed to start advertising, error code: %d", status_code);
    return;
  }
  ESP_LOGI(TAG, "advertising started!");
}

int set_fields_params(struct ble_hs_adv_fields *adv_fields,
                      struct ble_hs_adv_fields *rsp_fields,
                      struct ble_gap_adv_params *adv_params) {
  int status_code = 0;
  /* Set device name */
  const char *name = ble_svc_gap_device_name();
  rsp_fields->flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
  rsp_fields->name = (uint8_t *)name;
  rsp_fields->name_len = strlen(name);
  rsp_fields->name_is_complete = 1;

  rsp_fields->appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;
  rsp_fields->appearance_is_present = 1;

  rsp_fields->le_role = BLE_GAP_LE_ROLE_PERIPHERAL;
  rsp_fields->le_role_is_present = 1;

  /* Set scan response fields */
  status_code = ble_gap_adv_rsp_set_fields(rsp_fields);
  if (status_code != 0) {
    ESP_LOGE(TAG, "Failed to set scan response data, error code: %d",
             status_code);
    return status_code;
  }

  /* Set non-connetable and general discoverable mode to be a beacon */
  adv_params->conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params->disc_mode = BLE_GAP_DISC_MODE_GEN;

  /* Set advertising interval */
  adv_params->itvl_min = BLE_GAP_ADV_ITVL_MS(500);
  adv_params->itvl_max = BLE_GAP_ADV_ITVL_MS(510);
  return status_code;
}
