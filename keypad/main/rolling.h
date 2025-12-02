#ifndef ROLLING_CODES
#define ROLLING_CODES

#include "esp_system.h"

#define HMAC_KEY_SIZE 32
#define ROLLING_CODE_SIZE 8
#define ROLLING_CODE_DIGITS 6 // 6-digit decimal code for display
#define MAX_WINDOW_SIZE 100   // Accept codes within this window
#define MAX_FAILED_ATTEMPTS 5 // Lockout after this many failures

typedef struct {
  uint8_t key[HMAC_KEY_SIZE];
  uint64_t counter;
  uint64_t last_valid_counter;
  uint32_t failed_attempts;
} rolling_code_ctx_t;

esp_err_t rolling_code_init(rolling_code_ctx_t *ctx, const char *device_id);
uint32_t rolling_code_to_digits(const uint8_t *code, size_t len);
esp_err_t rolling_code_generate(rolling_code_ctx_t *ctx, uint64_t counter,
                                uint8_t *code_out, size_t code_len);
esp_err_t rolling_code_next(rolling_code_ctx_t *ctx, uint8_t *code_out);
esp_err_t rolling_code_next_digits(rolling_code_ctx_t *ctx,
                                   uint32_t *code_digits);
bool rolling_code_verify_digits(rolling_code_ctx_t *ctx,
                                uint32_t received_digits,
                                uint64_t received_counter);
bool rolling_code_verify(rolling_code_ctx_t *ctx, const uint8_t *received_code,
                         uint64_t received_counter);
bool rolling_code_verify_digits_auto(rolling_code_ctx_t *ctx,
                                     uint32_t received_digits);
void rolling_code_reset_lockout(rolling_code_ctx_t *ctx);
void rolling_code_print(const uint8_t *code, size_t len);
void rolling_key_print(const uint8_t *key);

#endif // !ROLLING_CODES
