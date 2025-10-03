#ifndef COMMON
#define COMMON

#include "esp_log.h"
#include "host/ble_gap.h"
#include <stddef.h>
#define TAG "NICE"
#define DEVICE_NAME "ID1"
#define SECRET_KEY "sTos5PGrvL1G9owYE8JITxY81wWo9ZnXtXPtB-vfGts"

struct CustStr {
  size_t len;
  char *pointer;
};

#endif
