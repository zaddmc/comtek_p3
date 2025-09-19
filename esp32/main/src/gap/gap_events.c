#include "gap/gap_events.h"
#include "commmon.h"
#include "gap/shared_gap.h"

int gap_connection_event_handler(
    struct ble_gap_event *event, void *arg,
    struct ble_gap_conn_desc *connection_description) {
  int status_code = 0;

  ESP_LOGI(TAG, "connection %s; status=%d",
           event->connect.status == 0 ? "established" : "failed",
           event->connect.status);

  if (event->connect.status != 0) {
    start_advertising();
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

  start_advertising();

  return status_code;
}
