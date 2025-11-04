#ifndef GAP_EVENTS
#define GAP_EVENTS

#include "host/ble_gap.h"
#include <stdint.h>
int gap_connection_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description);

int gap_disconnect_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description);

int gap_subscribe_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description);

extern uint8_t addr_val[6];
extern uint8_t own_addr_type;
extern uint8_t esp_uri[16];

void start_alt_advertising(void);

int set_fields_params(struct ble_hs_adv_fields *adv_fields,
                      struct ble_hs_adv_fields *rsp_fields,
                      struct ble_gap_adv_params *adv_params);

#endif
