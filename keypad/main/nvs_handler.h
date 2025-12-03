#ifndef NVS_HANDLER
#define NVS_HANDLER
#include "inttypes.h"

#define NVS_UNLOCKS "unlocks"
#define NVS_GOOGLE_KEY "google_find"

void init_nvs_handle(void);

void save_int(const char *key, const int32_t value);
int fetch_int(const char *key);

void save_string(const char *key, const char *value);
char *fetch_string(const char *key);

void rolling_save_u64(const char *key, const uint64_t value);

#endif // !NVS_HANDLER
