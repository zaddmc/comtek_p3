#include "gatt/write_svc_test.h"
#include "commmon.h"
#include "string.h"
#include "tools/led.h"
#include "tools/nvs_cust.h"
#include <stddef.h>
#include <stdio.h>

const ble_uuid16_t test_svc_uuid = BLE_UUID16_INIT(0x1815);
const ble_uuid16_t write_chr_uuid = BLE_UUID16_INIT(0x2A37);

uint16_t write_chr_handle;

int write_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                     struct ble_gatt_access_ctxt *ctxt, void *arg) {
  struct CustStr secret_key = nvs_read_key();
  size_t key_len = secret_key.len;
  if (key_len == 0) {
    ESP_LOGE(TAG, "COULD NOT READ KEY");
    free(secret_key.pointer);
    return -1;
  }
  int status_code = 0;

  /* Handle access events */
  /* Note: LED characteristic is write only */
  switch (ctxt->op) {

  /* Write characteristic event */
  case BLE_GATT_ACCESS_OP_WRITE_CHR:
    /* Verify connection handle */
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
      ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
               conn_handle, attr_handle);
    } else {
      ESP_LOGI(TAG, "characteristic write by nimble stack; attr_handle=%d",
               attr_handle);
    }

    /* Verify attribute handle */
    if (attr_handle == write_chr_handle) {
      printf("RECIVED DATA: ");
      for (uint16_t i = 0; i < ctxt->om->om_len; i++) {
        printf("%c", ctxt->om->om_data[i]);
      }
      printf("\n");
      int res = memcmp(ctxt->om->om_data, secret_key.pointer, key_len);
      if (res != 0) {
        led_on(255, 0, 0);
        printf("WRONG CODE\n");
      } else {
        led_on(0, 255, 0);
        printf("RIGHT CODE\n");
      }
      fflush(stdout);
    } else {
      status_code = BLE_ATT_ERR_UNLIKELY;
    }
    break;
  default:
    status_code = BLE_ATT_ERR_UNLIKELY;
    break;
  }
  if (status_code != 0) {
    ESP_LOGE(TAG, "WRONG acces to characteristic with optcode: %d", ctxt->op);
  }
  free(secret_key.pointer);
  return status_code;
}
