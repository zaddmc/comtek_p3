#include "ble_handler.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "keypad.h"
#include "nvs_flash.h"
#include "nvs_handler.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/* NimBLE */
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "rolling.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *TAG = "DUAL_ADV";

#define GOOGLE_FIND_MY_ADV_INSTANCE 0
#define WEBBLE_ADV_INSTANCE 1

static uint8_t google_find_my_key[20];

static const ble_uuid128_t webble_service_uuid =
    BLE_UUID128_INIT(0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34,
                     0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, );

static uint16_t webble_char_val_handle;
static char webble_data[50] = "Hello from ESP32!";

static char webble_response[50];
static bool webble_has_response = false;

static int hex_string_to_bytes(const char *hex_str, uint8_t *out,
                               size_t out_len) {
    size_t hex_len = strlen(hex_str);
    if (hex_len != out_len * 2) {
        ESP_LOGE(TAG, "Invalid hex string length: %d (expected %d)", hex_len,
                 out_len * 2);
        return -1;
    }
    for (size_t i = 0; i < out_len; i++) {
        char byte_str[3] = {hex_str[i * 2], hex_str[i * 2 + 1], '\0'};
        char *endptr;
        out[i] = (uint8_t)strtol(byte_str, &endptr, 16);
        if (*endptr != '\0') {
            ESP_LOGE(TAG, "Invalid hex character at position %d", i * 2);
            return -1;
        }
    }
    return 0;
}

static int webble_handle_command(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt) {
    const char tag[] = "WEBBLE_COMMAND";
    if (sizeof(webble_data) < 1) {
        ESP_LOGW(tag, "not enough data to take command");
        return 1;
    }
    webble_has_response = false;
    int command_len = strlen(webble_data);

    switch (webble_data[0]) {
    case 'u':
        if (command_len == 1) {
            ESP_LOGI(tag, "Command get unlocks was received");
            snprintf(webble_response, sizeof(webble_response), "%i",
                     fetch_int(NVS_UNLOCKS));
            os_mbuf_append(ctxt->om, &webble_response, sizeof(webble_response));
            webble_has_response = true;
            ble_gatts_notify(conn_handle, attr_handle);
        } else {
            ESP_LOGI(tag, "Command set unlocks was received");
            save_int(NVS_UNLOCKS, atoi(&webble_data[1]));
        }
        break;
    case 'g':
        if (command_len > 40 && command_len < 43) {
            ESP_LOGI(tag, "Command set google_find_my_key was received");
            save_string(NVS_GOOGLE_KEY, &webble_data[command_len - 40]);
            break;
        }
        // Default to return key
        ESP_LOGI(tag, "Command get google_find_my_key was received");
        snprintf(webble_response, sizeof(webble_response), "%s",
                 fetch_string(NVS_GOOGLE_KEY));
        os_mbuf_append(ctxt->om, &webble_response, sizeof(webble_response));
        webble_has_response = true;
        ble_gatts_notify(conn_handle, attr_handle);
        break;
    case 'c':
        ESP_LOGI(tag, "Command reset counter was received");
        rolling_save_u64("counter", 0);
        change_hmac_ctx(0);
        snprintf(webble_response, sizeof(webble_response), "Counter was reset");
        os_mbuf_append(ctxt->om, &webble_response, sizeof(webble_response));
        webble_has_response = true;
        ble_gatts_notify(conn_handle, attr_handle);
        break;
    case 'o':
        ESP_LOGI(tag, "Command reset counter was received");
        /* if (rolling_code_verify(&hmac_ctx, &webble_data[2],
                                atoi(&webble_data[19])))
            set_briefcase_state(true); */
        break;

    default:
        ESP_LOGI(tag, "Unrecognized command char: %c", webble_data[0]);
        break;
    }
    return 0;
}

static int webble_char_access(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        ESP_LOGI(TAG, "WebBLE: Read request");
        if (webble_has_response) {
            ESP_LOGI(TAG, "WebBLE: Returning response");
            os_mbuf_append(ctxt->om, &webble_response, sizeof(webble_response));
            webble_has_response = false;
        } else {
            ESP_LOGI(TAG, "WebBLE: Returning data");
            os_mbuf_append(ctxt->om, &webble_data, sizeof(webble_data));
        }
        return 0;
    case BLE_GATT_ACCESS_OP_WRITE_CHR: {
        ESP_LOGI(TAG, "WebBLE: Write request");
        memset(webble_data, '\0', sizeof(webble_data));
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);
        if (om_len > sizeof(webble_data))
            om_len = sizeof(webble_data);
        ble_hs_mbuf_to_flat(ctxt->om, webble_data, om_len, NULL);
        ESP_LOGI(TAG, "Received: %.*s", om_len, webble_data);
        webble_handle_command(conn_handle, attr_handle, ctxt);
        return 0;
    }
    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

static const struct ble_gatt_svc_def webble_services[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &webble_service_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    .uuid = BLE_UUID16_DECLARE(0x2A3D),
                    .access_cb = webble_char_access,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE |
                             BLE_GATT_CHR_F_NOTIFY,
                    .val_handle = &webble_char_val_handle,
                },
                {0},
            },
    },
    {0},
};
static void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt,
                                 void *arg) {
    char buf[BLE_UUID_STR_LEN];
    if (ctxt->op == BLE_GATT_REGISTER_OP_SVC) {
        ESP_LOGI(TAG, "Service %s registered",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf));
    }
}

// Build the CORRECT Google Find My Device Network advertisement
// According to Google FMDN spec: Service UUID is 0xFEAA, frame type 0x40
static int build_fmdn_advertisement(uint8_t *adv_data) {
    int len = 0;

    // Flags (required for discoverability)
    adv_data[len++] = 0x02; // Length of flags field
    adv_data[len++] = 0x01; // Flags AD type
    adv_data[len++] =
        0x06; // Flags: LE General Discoverable, BR/EDR not supported

    // Service Data for FMDN
    // Length = 1 (type) + 2 (UUID) + 1 (frame type) + 20 (EID) = 24 = 0x18
    adv_data[len++] = 0x18; // Length of service data field
    adv_data[len++] = 0x16; // Service Data AD type
    adv_data[len++] =
        0xAA; // 0xFEAA Service UUID (little endian) - FMDN/Eddystone
    adv_data[len++] = 0xFE; //
    adv_data[len++] = 0x40; // FMDN Frame type (0x40 = normal, 0x41 = UTP mode)

    // 20-byte Ephemeral Identifier (your key)
    memcpy(&adv_data[len], google_find_my_key, 20);
    len += 20;

    // Optional: Hashed flags byte (can be omitted if not using battery/UTP)
    // adv_data[len++] = 0x00;  // Uncomment if needed

    return len; // Should be 28 bytes total
}
// Start Google Find My Device advertisement using LEGACY advertising
// This is critical for compatibility - ESP32-S3 defaults to extended
// advertising
static void start_google_find_my_adv(void) {
    struct ble_gap_ext_adv_params params = {0};
    struct os_mbuf *data;
    ble_addr_t addr;
    uint8_t adv_data[31];
    int adv_data_len;
    int rc;

    ESP_LOGI(TAG, "Configuring Google Find My Device advertisement...");

    // CRITICAL: Use LEGACY PDU for compatibility with BLE 4.x scanners
    // Extended advertising PDUs may not be recognized by Android phones
    params.legacy_pdu = 1;  // Use legacy advertising PDU
    params.connectable = 0; // Non-connectable
    params.scannable = 0;   // Non-scannable
    params.anonymous = 0;
    params.include_tx_power = 0;
    params.scan_req_notif = 0;

    // Advertising interval: 20ms - 30ms for high visibility
    params.itvl_min = 0x20; // 20ms (32 * 0.625ms)
    params.itvl_max = 0x30; // 30ms (48 * 0.625ms)

    params.channel_map = 0x07;                  // All 3 advertising channels
    params.own_addr_type = BLE_OWN_ADDR_RANDOM; // Use random address
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127; // Max TX power

    // Step 1: Configure the advertising instance
    rc = ble_gap_ext_adv_configure(GOOGLE_FIND_MY_ADV_INSTANCE, &params, NULL,
                                   NULL, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to configure ext adv: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "Extended advertising configured");

    // Step 2: Set a static random address for this instance
    // Generate from the key for reproducibility
    memcpy(addr.val, google_find_my_key, 6);
    addr.val[5] |= 0xC0; // Set upper 2 bits for static random address
    addr.type = BLE_ADDR_RANDOM;

    rc = ble_gap_ext_adv_set_addr(GOOGLE_FIND_MY_ADV_INSTANCE, &addr);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set random address: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "Random address set: %02X:%02X:%02X:%02X:%02X:%02X",
             addr.val[5], addr.val[4], addr.val[3], addr.val[2], addr.val[1],
             addr.val[0]);

    // Step 3: Build the FMDN advertisement data
    adv_data_len = build_fmdn_advertisement(adv_data);

    ESP_LOGI(TAG, "FMDN Advertisement data (%d bytes):", adv_data_len);
    ESP_LOG_BUFFER_HEX(TAG, adv_data, adv_data_len);

    // Step 4: Allocate mbuf and set data
    data = os_msys_get_pkthdr(adv_data_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate mbuf");
        return;
    }

    rc = os_mbuf_append(data, adv_data, adv_data_len);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to append data: %d", rc);
        os_mbuf_free_chain(data);
        return;
    }

    rc = ble_gap_ext_adv_set_data(GOOGLE_FIND_MY_ADV_INSTANCE, data);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set adv data: %d", rc);
        return;
    }

    // Step 5: Start advertising indefinitely
    rc = ble_gap_ext_adv_start(GOOGLE_FIND_MY_ADV_INSTANCE, 0, 0);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start advertising: %d", rc);
        return;
    }
}
// Start WebBLE advertisement
static void start_webble_adv(void) {
    struct ble_gap_ext_adv_params params = {0};
    struct os_mbuf *data;
    uint8_t adv_data[31];
    int adv_data_len = 0;
    int rc;

    params.connectable = 1;
    params.scannable = 1;
    params.legacy_pdu = 1;
    params.itvl_min = 0x40;
    params.itvl_max = 0x60;
    params.channel_map = 0x07;
    params.own_addr_type = BLE_OWN_ADDR_PUBLIC;
    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127;

    rc = ble_gap_ext_adv_configure(WEBBLE_ADV_INSTANCE, &params, NULL, NULL,
                                   NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to configure WebBLE adv: %d", rc);
        return;
    }

    // Flags
    adv_data[adv_data_len++] = 0x02;
    adv_data[adv_data_len++] = 0x01;
    adv_data[adv_data_len++] = 0x06;

    // Complete name
    const char *name = "ESP32-WebBLE";
    uint8_t name_len = strlen(name);
    adv_data[adv_data_len++] = name_len + 1;
    adv_data[adv_data_len++] = 0x09;
    memcpy(&adv_data[adv_data_len], name, name_len);
    adv_data_len += name_len;

    data = os_msys_get_pkthdr(adv_data_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate mbuf for WebBLE");
        return;
    }

    rc = os_mbuf_append(data, adv_data, adv_data_len);
    if (rc != 0) {
        os_mbuf_free_chain(data);
        return;
    }

    rc = ble_gap_ext_adv_set_data(WEBBLE_ADV_INSTANCE, data);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set WebBLE data: %d", rc);
        return;
    }

    rc = ble_gap_ext_adv_start(WEBBLE_ADV_INSTANCE, 0, 0);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start WebBLE adv: %d", rc);
        return;
    }

    ESP_LOGI(TAG, "WebBLE advertisement started");
}

static int gap_event_cb(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "Connection %s; status=%d",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);
        if (event->connect.status != 0) {
            start_webble_adv();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnect; reason=%d", event->disconnect.reason);
        start_webble_adv();
        break;
    default:
        ESP_LOGI(TAG, "Unknown event");
        break;
    }
    return 0;
}

static void nimble_host_task(void *param) {
    ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

static void ble_on_sync(void) {
    int rc;

    ESP_LOGI(TAG, "BLE stack synced - starting advertisements");

    // Ensure we have a valid address
    rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        ESP_LOGE(TAG, "ble_hs_util_ensure_addr failed: %d", rc);
    }

    rc = ble_svc_gap_device_name_set("ESP32-S3-Dual");
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set device name: %d", rc);
    }

    // Add a small delay to ensure BLE stack is fully ready
    vTaskDelay(pdMS_TO_TICKS(100));

    // Start advertisements
    start_google_find_my_adv();
    start_webble_adv();
}

static void ble_on_reset(int reason) {
    ESP_LOGE(TAG, "BLE reset; reason=%d", reason);
}

static int gatt_svr_init(void) {
    int rc;
    ble_svc_gap_init();
    ble_svc_gatt_init();
    rc = ble_gatts_count_cfg(webble_services);
    if (rc != 0)
        return rc;
    rc = ble_gatts_add_svcs(webble_services);
    return rc;
}

void start_dual_advertising(void) {
    int rc;

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "ESP32-S3 Dual Advertisement (NimBLE)");
    ESP_LOGI(TAG, "Google Find My Device + WebBLE");
    ESP_LOGI(TAG, "========================================");

    // Convert hex string key to bytes

    rc = hex_string_to_bytes(fetch_string("google_find"), google_find_my_key,
                             sizeof(google_find_my_key));
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to parse Google Find My key!");
        return;
    }

    ESP_LOGI(TAG, "Ephemeral ID (EID) loaded:");
    ESP_LOG_BUFFER_HEX(TAG, google_find_my_key, sizeof(google_find_my_key));

    // Initialize NimBLE
    ESP_ERROR_CHECK(nimble_port_init());

    // Configure NimBLE host
    ble_hs_cfg.reset_cb = ble_on_reset;
    ble_hs_cfg.sync_cb = ble_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    // Initialize GATT server
    rc = gatt_svr_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initialize GATT server: %d", rc);
        return;
    }

    // Start NimBLE host task
    nimble_port_freertos_init(nimble_host_task);

    ESP_LOGI(TAG, "Initialization complete, waiting for BLE sync...");
}

void stop_advertisement(void) {
    ESP_LOGI(TAG, "Stopping BLE advertisement");
    ble_gap_adv_stop();
    nimble_port_stop();
    nimble_port_deinit();
}
