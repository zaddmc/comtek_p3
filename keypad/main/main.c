#include "ble_handler.h"
#include "esp_log.h"
#include "keypad.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "ssd1306.h"

nvs_handle_t init_nvs_handle(void) {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS handle
    nvs_handle_t handle;
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE("MAIN_NVS", "Error (%s) opening NVS handle!",
                 esp_err_to_name(err));
    }
    return handle;
}

#define PIN_NUM_MOSI 35
#define PIN_NUM_CLK 36
#define PIN_NUM_DC 20
#define PIN_NUM_RST 47

static SSD1306_t display;

SSD1306_t *init_display_device(void) {
    spi_master_init(&display, PIN_NUM_MOSI, PIN_NUM_CLK, -1, PIN_NUM_DC,
                    PIN_NUM_RST);
    ssd1306_init(&display, 128, 32); // Lower number to increase font size

    // Clear the screen
    ssd1306_clear_screen(&display, false);
    return &display;

    // Display some text
    ssd1306_display_text(&display, 0, "Hello, ESP32-S3!", 16, false);
    ssd1306_display_text(&display, 1, "SSD1306 (SPI)", 13, false);

    int itterator = 0;
    char buf[16];
    while (1) {
        snprintf(buf, sizeof(buf), "%i", itterator++);
        ssd1306_display_text(&display, 3, buf, strlen(buf), false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    ESP_LOGI("MAIN", "Initializing NVS");
    nvs_handle_t handle = init_nvs_handle();

    ESP_LOGI("MAIN", "Initializing SSD1306 Display");
    SSD1306_t *display = init_display_device();

    ssd1306_display_text(display, 0, "Hello, Patteren!", 16, false);

    ESP_LOGI("MAIN", "STARTING DUAL ADVERTISEMENT");
    start_dual_advertising();

    ESP_LOGI("MAIN", "Starting Keypad");
    keypad_main(handle, display);
}
