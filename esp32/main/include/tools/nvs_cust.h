#ifndef NVS_CUST
#define NVS_CUST
#include "commmon.h"
#include "esp_log.h"
#include "nvs.h"

int32_t nvs_read_counter();
esp_err_t nvs_write_counter(int32_t new_counter);
struct CustStr nvs_read_key();
esp_err_t nvs_init_cust();
esp_err_t nvs_initialize_key();
esp_err_t nvs_initialize_counter();

esp_err_t nvs_erase_main_keys();

#endif
