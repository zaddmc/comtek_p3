#include "gap_events.h"
#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "host/util/util.h"
#include "location.h"
#include "services/gap/ble_svc_gap.h"

#define TAG "GAP_HANDLER"
#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0200
#define BLE_GAP_URI_PREFIX_HTTPS 0x17
#define BLE_GAP_LE_ROLE_PERIPHERAL 0x00

int gap_connection_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description) {
    int status_code = 0;

    ESP_LOGI(TAG, "connection %s; status=%d",
             event->connect.status == 0 ? "established" : "failed",
             event->connect.status);

    if (event->connect.status != 0) {
        return status_code;
    }

    status_code =
        ble_gap_conn_find(event->connect.conn_handle, connection_description);
    if (status_code != 0) {
        ESP_LOGE(TAG, "Failed to find connection with code: %d", status_code);
        return status_code;
    }

    struct ble_gap_upd_params params = {
        .itvl_min = connection_description->conn_itvl,
        .itvl_max = connection_description->conn_itvl,
        .latency = 3,
        .supervision_timeout = connection_description->supervision_timeout};
    status_code = ble_gap_update_params(event->connect.conn_handle, &params);
    if (status_code != 0) {
        ESP_LOGE(TAG, "Failed to update params with code: %d", status_code);
        return status_code;
    }
    return status_code;
};
int gap_disconnect_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description) {

    int status_code = 0;

    ESP_LOGI(TAG, "Disconnect: reason=%d", event->disconnect.reason);

    return status_code;
}

int gap_subscribe_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description) {

    ESP_LOGI(TAG, "Subsribe event");

    return 0;
}

int set_fields_params(struct ble_hs_adv_fields *adv_fields,
                      struct ble_hs_adv_fields *rsp_fields,
                      struct ble_gap_adv_params *adv_params);

void start_alt_advertising(void) {
    int status_code = 0;
    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields rsp_fields = {0};
    struct ble_gap_adv_params adv_params = {0};

    status_code = set_fields_params(&adv_fields, &rsp_fields, &adv_params);

    status_code = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                                    &adv_params, ble_advertise_cb, NULL);
    if (status_code != 0) {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d",
                 status_code);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}

int set_fields_params(struct ble_hs_adv_fields *adv_fields,
                      struct ble_hs_adv_fields *rsp_fields,
                      struct ble_gap_adv_params *adv_params) {
    int status_code = 0;

    /* Set advertising flags */
    adv_fields->flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Set device name */
    const char *name = ble_svc_gap_device_name();
    adv_fields->name = (uint8_t *)name;
    adv_fields->name_len = strlen(name);
    adv_fields->name_is_complete = 1;

    /* Set device tx power */
    adv_fields->tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    adv_fields->tx_pwr_lvl_is_present = 1;

    /* Set device appearance */
    adv_fields->appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;
    adv_fields->appearance_is_present = 1;

    /* Set device LE role */
    adv_fields->le_role = BLE_GAP_LE_ROLE_PERIPHERAL;
    adv_fields->le_role_is_present = 1;

    /* Set advertiement fields */
    status_code = ble_gap_adv_set_fields(adv_fields);
    if (status_code != 0) {
        ESP_LOGE(TAG, "Failed to set advertising data, error code: %d",
                 status_code);
        return status_code;
    }

    /* Set device address */
    rsp_fields->device_addr = addr_val;
    rsp_fields->device_addr_type = own_addr_type;
    rsp_fields->device_addr_is_present = 1;

    /* Set URI */
    rsp_fields->uri = esp_uri;
    rsp_fields->uri_len = sizeof(esp_uri);

    /* Set advertising interval */
    rsp_fields->adv_itvl = BLE_GAP_ADV_ITVL_MS(500);
    rsp_fields->adv_itvl_is_present = 1;

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
#undef TAG
