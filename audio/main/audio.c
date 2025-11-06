#include "driver/i2s.h"
#include "driver/i2s_pdm.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"
#include "driver/i2s_types_legacy.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define TAG "WAV_PLAYER"

static void i2s_init(void) {
    static const i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = 8000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_desc_num = 8,
        .dma_frame_num = 256,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    static const i2s_pin_config_t pin_config = {
        .bck_io_num = 5,
        .ws_io_num = 6,
        .data_out_num = 7,
        .data_in_num = 8,
    };
    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM_0, &pin_config));
    ESP_ERROR_CHECK(i2s_set_clk(I2S_NUM_0, i2s_config.sample_rate,
                                i2s_config.bits_per_sample,
                                i2s_config.channel_format));
}

static void spiffs_init(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    size_t total = 0, used = 0;
    esp_spiffs_info(conf.partition_label, &total, &used);
    ESP_LOGI(TAG, "SPIFFS total: &d bytes, used: %d bytes", total, used);
}
static void play_wav(const char *filename) {
    FILE *fptr = fopen(filename, "rb");
    if (!fptr) {
        ESP_LOGE(TAG, "Failed to open %s", filename);
        return;
    }

    // 44 is WAV header size, dont ask me why
    fseek(fptr, 44, SEEK_SET);

    uint8_t buffer[1024];
    size_t bytes_read = 0, bytes_written = 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fptr)) > 0) {
        i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
    }
    fclose(fptr);
    ESP_LOGI(TAG, "Playback complete");
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing I2S and spiffs...");
    i2s_init();
    spiffs_init();
    while (1) {
        ESP_LOGI(TAG, "playing /spiffs/test.wav");
        play_wav("/spiffs/test.wav");
    }
}
