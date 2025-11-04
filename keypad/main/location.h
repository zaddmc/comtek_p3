#ifndef LOCATION_H
#define LOCATION_H
#include "host/ble_hs.h"
#include "nvs.h"

/* Will post its location to google nova api thing */
void location_main(nvs_handle_t handle);
int ble_advertise_cb(struct ble_gap_event *event, void *arg);

#endif
