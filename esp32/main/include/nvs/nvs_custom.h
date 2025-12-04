#ifndef NVS_CUST
#define NVS_CUST
#include "commmon.h"
#include "esp_log.h"
#include "nvs.h"
#define NVS_ROLLING_CODE_KEY_TAG "roll_key"
#define NVS_ROLLING_CODE_COUNTER_TAG "roll_c"
#define NVS_KEYPAD_CODE_TAG "kp_c"
#define NVS_KEYPAD_TICKETS_TAG "kp_t"

esp_err_t nvs_init_custom();
int32_t nvs_read_i32_custom(char *key);
esp_err_t nvs_write_i32_custom(char *key, int32_t val);
struct CustStr nvs_read_string_custom(char *key);
esp_err_t nvs_write_string_custom(char *key, char *val);
esp_err_t nvs_erase_key_custom(char *key);

esp_err_t nvs_initialize_custom_keys();
esp_err_t nvs_erase_custom_keys();

esp_err_t nvs_initialize_string_key(char *key, char *val);
esp_err_t nvs_initialize_i32_key(char *key, int32_t val);

#endif
