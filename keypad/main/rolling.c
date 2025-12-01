#include "esp_log.h"
#include "esp_random.h"
#include "esp_system.h"
#include "mbedtls/md.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "rolling.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "HMAC_ROLLING";

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

    ESP_LOGW(TAG, "Invalid code for counter: %llu", received_counter);
    return false;
}

void rolling_code_print(const uint8_t *code, size_t len) {
    printf("Code: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02X", code[i]);
    }
    printf("\n");
}
