#ifndef GATT_W_SVC
#define GATT_W_SVC

#include "host/ble_gatt.h"
#include <stdint.h>

extern const ble_uuid16_t test_svc_uuid;
extern const ble_uuid16_t write_chr_uuid;

extern uint16_t write_chr_handle;

int write_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                     struct ble_gatt_access_ctxt *ctxt, void *arg);

#endif
