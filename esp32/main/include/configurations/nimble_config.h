#ifndef NIMBLE_C
#define NIMBLE_C

#include "nimble/nimble_port.h"
#include "nvs_flash.h"

void nimble_host_config_init(void);

void nimble_host_task(void *param);

#endif
