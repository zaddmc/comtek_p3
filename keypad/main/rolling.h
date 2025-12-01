#ifndef ROLLING_CODES
#define ROLLING_CODES

#include "esp_system.h"

#define HMAC_KEY_SIZE 32
#define ROLLING_CODE_SIZE 8
#define MAX_WINDOW_SIZE 100 // Accept codes within this window

typedef struct {
  uint8_t key[HMAC_KEY_SIZE];
  uint64_t counter;
  uint64_t last_valid_counter;
} rolling_code_ctx_t;

esp_err_t rolling_code_init(rolling_code_ctx_t *ctx, const char *device_id);
esp_err_t rolling_code_generate(rolling_code_ctx_t *ctx, uint64_t counter,
                                uint8_t *code_out, size_t code_len);
esp_err_t rolling_code_next(rolling_code_ctx_t *ctx, uint8_t *code_out);
bool rolling_code_verify(rolling_code_ctx_t *ctx, const uint8_t *received_code,
                         uint64_t received_counter);
void rolling_code_print(const uint8_t *code, size_t len);

#endif // !ROLLING_CODES
