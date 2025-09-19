#ifndef GAP_E_H
#define GAP_E_H

#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0200
#define BLE_GAP_URI_PREFIX_HTTPS 0x17
#define BLE_GAP_LE_ROLE_PERIPHERAL 0x00
#define TAG "SKIBIDI_TAG"

#include "esp_log.h"
#include "host/ble_gap.h"

int gap_event_handler(struct ble_gap_event *event, void *arg);

#endif
