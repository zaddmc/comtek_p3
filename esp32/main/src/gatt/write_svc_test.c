#include "gatt/write_svc_test.h"
#include "commmon.h"
#include "esp_err.h"
#include "host/ble_gatt.h"
#include "os/os_mbuf.h"
#include "string.h"
#include "tools/led.h"
#include "tools/sha256.h"
#include <nvs/nvs_custom.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

const ble_uuid16_t test_svc_uuid = BLE_UUID16_INIT(0x1815);
const ble_uuid16_t write_chr_uuid = BLE_UUID16_INIT(0x2A37);

uint16_t write_chr_handle;
static uint8_t write_chr_val[2] = {0};
static uint16_t write_chr_handle_conn = 0;

int write_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                     struct ble_gatt_access_ctxt *ctxt, void *arg) {

  int32_t counter = nvs_read_i32_custom(NVS_ROLLING_CODE_COUNTER_TAG);
  if (counter < 0) {
    ESP_LOGE(TAG, "COULD NOT READ COUNTER");
    return -1;
  }
  struct CustStr secret_key = nvs_read_string_custom(NVS_ROLLING_CODE_KEY_TAG);
  size_t key_len = secret_key.len;
  if (key_len == 0) {
    ESP_LOGE(TAG, "COULD NOT READ KEY");
    free(secret_key.pointer);
    return -1;
  }

  int status_code = 0;

  switch (ctxt->op) {

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
      int32_t start_counter = counter;
      char hashed_key[65];
      bool found_correct_key = false;
      for (; counter < start_counter + ROLLING_CODE_EXTRA_COUNT; counter++) {
        RollingSHA256(hashed_key, secret_key.pointer, counter, secret_key.len,
                      sizeof(counter));
        int res = memcmp(ctxt->om->om_data, hashed_key, SHA256_OUT_LEN - 1);
        if (res == 0) {
          found_correct_key = true;
          break;
        }
      }
      if (!found_correct_key) {
        led_on(50, 0, 0);
        printf("WRONG CODE\n");
        write_chr_val[1] = 1;
      } else {
        led_on(0, 50, 0);
        printf("RIGHT CODE\n");
        printf("COUNTER %d\n", (int)counter);
        write_chr_val[1] = 0;
        counter += 1;
        if (nvs_write_i32_custom(NVS_ROLLING_CODE_COUNTER_TAG, counter) !=
            ESP_OK) {
          write_chr_val[1] = -1;
          status_code = BLE_ATT_ERR_UNLIKELY;
        }
      }
      fflush(stdout);
      status_code =
          os_mbuf_append(ctxt->om, &write_chr_val, sizeof(write_chr_val));
      ble_gatts_notify(conn_handle, write_chr_handle);
    } else {
      status_code = BLE_ATT_ERR_UNLIKELY;
    }
    break;
  case BLE_GATT_ACCESS_OP_READ_CHR:
    status_code =
        os_mbuf_append(ctxt->om, &write_chr_val, sizeof(write_chr_val));
    break;
  default:
    status_code = BLE_ATT_ERR_UNLIKELY;
    break;
  }
  if (status_code != 0) {
    ESP_LOGE(TAG,
             "WRONG acces to characteristic with optcode: %d STATUS CODE: %d",
             ctxt->op, status_code);
  }
  free(secret_key.pointer);
  return status_code;
}

void write_svc_subscribe_cb(struct ble_gap_event *event) {
  if (event->subscribe.attr_handle == write_chr_handle) {
    write_chr_handle_conn = event->subscribe.conn_handle;
  }
}
