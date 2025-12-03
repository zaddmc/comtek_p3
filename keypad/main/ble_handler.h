#ifndef BLE_HANDLER
#define BLE_HANDLER

#include "rolling.h"

void start_dual_advertising(void);
void stop_advertisement(void);

extern rolling_code_ctx_t hmac_ctx;

#endif // BLE_HANDLER
