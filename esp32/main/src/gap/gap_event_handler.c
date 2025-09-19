#include "gap/gap_event_handler.h"
#include "gap/gap_events.h"

int gap_event_handler(struct ble_gap_event *event, void *arg) {
  int status_code = 0;
  struct ble_gap_conn_desc connection_description;

  switch (event->type) {

  case BLE_GAP_EVENT_CONNECT:
    return gap_connection_event_handler(event, arg, &connection_description);
  case BLE_GAP_EVENT_DISCONNECT:
    return gap_disconnect_event_handler(event, arg, &connection_description);
  }
  return status_code;
}
