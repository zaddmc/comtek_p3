#ifndef GAP_S
#define GAP_S

#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0200
#define BLE_GAP_URI_PREFIX_HTTPS 0x17
#define BLE_GAP_LE_ROLE_PERIPHERAL 0x00

#include "host/ble_gap.h"
#include "host/ble_hs_adv.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

extern uint8_t own_addr_type;
extern uint8_t addr_val[6];
extern uint8_t esp_uri[16];

void start_advertising();

int set_fields_params(struct ble_hs_adv_fields *adv_fields,
                      struct ble_hs_adv_fields *rsp_fields,
                      struct ble_gap_adv_params *adv_params);

#endif
