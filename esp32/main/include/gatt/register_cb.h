#ifndef REGISTER_CB
#define REGISTER_CB

#include "host/ble_gatt.h"
#include "host/ble_uuid.h"

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

#endif
