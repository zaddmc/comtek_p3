#ifndef GAP_EVENTS

#define GAP_EVENTS

#include "host/ble_gap.h"
int gap_connection_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description);

int gap_disconnect_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description);

#endif
