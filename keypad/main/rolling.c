#include "esp_log.h"
#include "esp_random.h"
#include "esp_system.h"
#include "mbedtls/md.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "rolling.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "HMAC_ROLLING";

// Function to convert a hex string into a byte array
void hex_string_to_bytes(const char *hex, uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        sscanf(hex + 2 * i, "%2hhx", &bytes[i]);
    }
}

/**
 * Initialize NVS and load/create rolling code context
 */
esp_err_t rolling_code_init(rolling_code_ctx_t *ctx, const char *device_id) {
    esp_err_t err;
    nvs_handle_t nvs_handle;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS
    err = nvs_open("rolling_code", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return err;
    }

    // Try to load existing key and counter
    size_t key_size = HMAC_KEY_SIZE;
    err = nvs_get_blob(nvs_handle, "hmac_key", ctx->key, &key_size);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Generate new key
        ESP_LOGI(TAG, "Generating new HMAC key for device: %s", device_id);
        esp_fill_random(ctx->key, HMAC_KEY_SIZE);

        // Save key
        err = nvs_set_blob(nvs_handle, "hmac_key", ctx->key, HMAC_KEY_SIZE);
        if (err != ESP_OK) {
            nvs_close(nvs_handle);
            return err;
        }

        ctx->counter = 0;
        ctx->last_valid_counter = 0;
        ctx->failed_attempts = 0;

        err = nvs_set_u64(nvs_handle, "counter", ctx->counter);
        if (err != ESP_OK) {
            nvs_close(nvs_handle);
            return err;
        }

        nvs_commit(nvs_handle);
    } else if (err == ESP_OK) {
        // Load counter
        err = nvs_get_u64(nvs_handle, "counter", &ctx->counter);
        if (err != ESP_OK) {
            ctx->counter = 0;
        }
        ctx->last_valid_counter = ctx->counter;
        ESP_LOGI(TAG, "Loaded existing key, counter: %llu", ctx->counter);
    }

    nvs_close(nvs_handle);
    return ESP_OK;
}

/**
 * Convert HMAC output to 6-digit decimal code
 */
uint32_t rolling_code_to_digits(const uint8_t *code, size_t len) {
    // Use first 4 bytes as uint32
    uint32_t value = 0;
    for (int i = 0; i < 4 && i < len; i++) {
        value = (value << 8) | code[i];
    }
    // Modulo to get 6 digits (0-999999)
    return value % 1000000;
}

/**
 * Generate HMAC-based rolling code
 */
esp_err_t rolling_code_generate(rolling_code_ctx_t *ctx, uint64_t counter,
                                uint8_t *code_out, size_t code_len) {
    if (code_len != ROLLING_CODE_SIZE) {
        return ESP_ERR_INVALID_SIZE;
    }

    mbedtls_md_context_t md_ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
    uint8_t hmac_result[32];

    mbedtls_md_init(&md_ctx);

    esp_err_t ret = ESP_OK;
    if (mbedtls_md_setup(&md_ctx, mbedtls_md_info_from_type(md_type), 1) != 0) {
        ret = ESP_FAIL;
        goto cleanup;
    }

    if (mbedtls_md_hmac_starts(&md_ctx, ctx->key, HMAC_KEY_SIZE) != 0) {
        ret = ESP_FAIL;
        goto cleanup;
    }

    // Use counter as input
    uint8_t counter_bytes[8];
    for (int i = 0; i < 8; i++) {
        counter_bytes[i] = (counter >> (56 - i * 8)) & 0xFF;
    }

    if (mbedtls_md_hmac_update(&md_ctx, counter_bytes, 8) != 0) {
        ret = ESP_FAIL;
        goto cleanup;
    }

    if (mbedtls_md_hmac_finish(&md_ctx, hmac_result) != 0) {
        ret = ESP_FAIL;
        goto cleanup;
    }

    // Use first 8 bytes as rolling code
    memcpy(code_out, hmac_result, ROLLING_CODE_SIZE);

cleanup:
    mbedtls_md_free(&md_ctx);
    return ret;
}

/**
 * Get next rolling code (increments counter)
 */
esp_err_t rolling_code_next(rolling_code_ctx_t *ctx, uint8_t *code_out) {
    esp_err_t err =
        rolling_code_generate(ctx, ctx->counter, code_out, ROLLING_CODE_SIZE);
    if (err != ESP_OK) {
        return err;
    }

    ctx->counter++;

    // Save updated counter to NVS
    nvs_handle_t nvs_handle;
    err = nvs_open("rolling_code", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_set_u64(nvs_handle, "counter", ctx->counter);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }

    return ESP_OK;
}

/**
 * Get next rolling code as 6-digit number (increments counter)
 */
esp_err_t rolling_code_next_digits(rolling_code_ctx_t *ctx,
                                   uint32_t *code_digits) {
    uint8_t code[ROLLING_CODE_SIZE];
    esp_err_t err = rolling_code_next(ctx, code);
    if (err != ESP_OK) {
        return err;
    }

    *code_digits = rolling_code_to_digits(code, ROLLING_CODE_SIZE);
    return ESP_OK;
}

/**
 * Verify received rolling code (6-digit version)
 */
bool rolling_code_verify_digits(rolling_code_ctx_t *ctx,
                                uint32_t received_digits,
                                uint64_t received_counter) {
    // Check if locked out due to failed attempts
    if (ctx->failed_attempts >= MAX_FAILED_ATTEMPTS) {
        ESP_LOGE(TAG, "Device locked due to too many failed attempts");
        return false;
    }

    // Check if counter is within acceptable window
    if (received_counter <= ctx->last_valid_counter) {
        ESP_LOGW(TAG, "Counter replay detected: %llu <= %llu", received_counter,
                 ctx->last_valid_counter);
        ctx->failed_attempts++;
        return false;
    }

    if (received_counter > ctx->last_valid_counter + MAX_WINDOW_SIZE) {
        ESP_LOGW(TAG, "Counter too far ahead: %llu > %llu + %d",
                 received_counter, ctx->last_valid_counter, MAX_WINDOW_SIZE);
        ctx->failed_attempts++;
        return false;
    }

    // Generate expected code
    uint8_t expected_code[ROLLING_CODE_SIZE];
    if (rolling_code_generate(ctx, received_counter, expected_code,
                              ROLLING_CODE_SIZE) != ESP_OK) {
        ctx->failed_attempts++;
        return false;
    }

    uint32_t expected_digits =
        rolling_code_to_digits(expected_code, ROLLING_CODE_SIZE);

    if (expected_digits == received_digits) {
        ctx->last_valid_counter = received_counter;
        ctx->failed_attempts = 0; // Reset on success
        ESP_LOGI(TAG, "Valid code accepted, counter: %llu", received_counter);

        // Update NVS
        nvs_handle_t nvs_handle;
        if (nvs_open("rolling_code", NVS_READWRITE, &nvs_handle) == ESP_OK) {
            nvs_set_u64(nvs_handle, "counter", ctx->last_valid_counter);
            nvs_set_u32(nvs_handle, "failed_attempts", ctx->failed_attempts);
            nvs_commit(nvs_handle);
            nvs_close(nvs_handle);
        }
        return true;
    }

    ctx->failed_attempts++;
    ESP_LOGW(TAG, "Invalid code for counter: %llu (attempt %d/%d)",
             received_counter, ctx->failed_attempts, MAX_FAILED_ATTEMPTS);

    // Save failed attempts
    nvs_handle_t nvs_handle;
    if (nvs_open("rolling_code", NVS_READWRITE, &nvs_handle) == ESP_OK) {
        nvs_set_u32(nvs_handle, "failed_attempts", ctx->failed_attempts);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }

    return false;
}

/**
 * Reset failed attempts counter (admin function)
 */
void rolling_code_reset_lockout(rolling_code_ctx_t *ctx) {
    ctx->failed_attempts = 0;
    ESP_LOGI(TAG, "Lockout reset");

    nvs_handle_t nvs_handle;
    if (nvs_open("rolling_code", NVS_READWRITE, &nvs_handle) == ESP_OK) {
        nvs_set_u32(nvs_handle, "failed_attempts", 0);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

/**
 * Verify received rolling code
 */
bool rolling_code_verify(rolling_code_ctx_t *ctx, const uint8_t *received_code,
                         uint64_t received_counter) {
    // Check if counter is within acceptable window
    if (received_counter <= ctx->last_valid_counter) {
        ESP_LOGW(TAG, "Counter replay detected: %llu <= %llu", received_counter,
                 ctx->last_valid_counter);
        return false;
    }

    if (received_counter > ctx->last_valid_counter + MAX_WINDOW_SIZE) {
        ESP_LOGW(TAG, "Counter too far ahead: %llu > %llu + %d",
                 received_counter, ctx->last_valid_counter, MAX_WINDOW_SIZE);
        return false;
    }

    // Generate expected code
    uint8_t expected_code[ROLLING_CODE_SIZE];
    if (rolling_code_generate(ctx, received_counter, expected_code,
                              ROLLING_CODE_SIZE) != ESP_OK) {
        return false;
    }

    // Constant-time comparison
    int match = 1;
    for (int i = 0; i < ROLLING_CODE_SIZE; i++) {
        match &= (expected_code[i] == received_code[i]);
    }

    if (match) {
        ctx->last_valid_counter = received_counter;
        ESP_LOGI(TAG, "Valid code accepted, counter: %llu", received_counter);

        // Update NVS
        nvs_handle_t nvs_handle;
        if (nvs_open("rolling_code", NVS_READWRITE, &nvs_handle) == ESP_OK) {
            nvs_set_u64(nvs_handle, "counter", ctx->last_valid_counter);
            nvs_commit(nvs_handle);
            nvs_close(nvs_handle);
        }
        return true;
    }

    // ESP_LOGW(TAG, "Invalid code for counter: %llu", received_counter);
    return false;
}

/**
 * Verify code without knowing exact counter (searches window)
 */
bool rolling_code_verify_auto(rolling_code_ctx_t *ctx,
                              const char *received_code) {
    // Try counter values in window: [last_valid + 1] to [last_valid +
    // MAX_WINDOW_SIZE]
    uint64_t start_counter = ctx->last_valid_counter + 1;
    uint64_t end_counter = ctx->last_valid_counter + MAX_WINDOW_SIZE;

    ESP_LOGI(TAG, "Searching for code in counter range %llu to %llu",
             start_counter, end_counter);

    uint8_t received_hex[ROLLING_CODE_SIZE];
    hex_string_to_bytes(received_code, received_hex, ROLLING_CODE_SIZE);

    for (uint64_t test_counter = start_counter; test_counter <= end_counter;
         test_counter++) {
        if (rolling_code_verify(ctx, received_hex, test_counter)) {
            return true;
        }
    }
    return false;
}

/**
 * Verify 6-digit code without knowing exact counter (searches window)
 * Used when only the code is manually entered without counter info
 */
bool rolling_code_verify_digits_auto(rolling_code_ctx_t *ctx,
                                     uint32_t received_digits) {
    // Check if locked out due to failed attempts
    if (ctx->failed_attempts >= MAX_FAILED_ATTEMPTS && false) {
        ESP_LOGE(TAG, "Device locked due to too many failed attempts");
        return false;
    }

    // Try counter values in window: [last_valid + 1] to [last_valid +
    // MAX_WINDOW_SIZE]
    uint64_t start_counter = ctx->last_valid_counter + 1;
    uint64_t end_counter = ctx->last_valid_counter + MAX_WINDOW_SIZE;

    ESP_LOGI(TAG, "Searching for code in counter range %llu to %llu",
             start_counter, end_counter);

    for (uint64_t test_counter = start_counter; test_counter <= end_counter;
         test_counter++) {
        // Generate expected code for this counter
        uint8_t expected_code[ROLLING_CODE_SIZE];
        if (rolling_code_generate(ctx, test_counter, expected_code,
                                  ROLLING_CODE_SIZE) != ESP_OK) {
            continue;
        }

        uint32_t expected_digits =
            rolling_code_to_digits(expected_code, ROLLING_CODE_SIZE);

        if (expected_digits == received_digits) {
            // Found matching code!
            ctx->last_valid_counter = test_counter;
            ctx->failed_attempts = 0; // Reset on success

            ESP_LOGI(TAG, "Valid code found at counter: %llu", test_counter);

            // Update NVS
            nvs_handle_t nvs_handle;
            if (nvs_open("rolling_code", NVS_READWRITE, &nvs_handle) ==
                ESP_OK) {
                nvs_set_u64(nvs_handle, "counter", ctx->last_valid_counter);
                nvs_set_u32(nvs_handle, "failed_attempts",
                            ctx->failed_attempts);
                nvs_commit(nvs_handle);
                nvs_close(nvs_handle);
            }
            return true;
        }
    }

    // No match found in window
    ctx->failed_attempts++;
    ESP_LOGW(TAG, "No valid code found in window (attempt %d/%d)",
             ctx->failed_attempts, MAX_FAILED_ATTEMPTS);

    // Save failed attempts
    nvs_handle_t nvs_handle;
    if (nvs_open("rolling_code", NVS_READWRITE, &nvs_handle) == ESP_OK) {
        nvs_set_u32(nvs_handle, "failed_attempts", ctx->failed_attempts);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }

    return false;
}

/**
 * Print code in hex format
 */
void rolling_code_print(const uint8_t *code, size_t len) {
    printf("Code: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02X", code[i]);
    }
    printf("\n");
}

void rolling_key_print(const uint8_t *key) {
    printf("Key: ");
    for (size_t i = 0; i < HMAC_KEY_SIZE; i++) {
        printf("%02X", key[i]);
    }
    printf("\n");
}
