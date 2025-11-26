#ifndef NVS_HANDLER
#define NVS_HANDLER
#include "inttypes.h"

void init_nvs_handle(void);

void save_int(const char *key, const int32_t value);
int fetch_int(const char *key);

void save_string(const char *key, const char *value);
char *fetch_string(const char *key);

#endif // !NVS_HANDLER
