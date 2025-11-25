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

#define PIN_NUM_CLK 36  // SCK
#define PIN_NUM_MOSI 35 // SDA
#define PIN_NUM_RST 47  // RES
#define PIN_NUM_DC 20   // DC

static SSD1306_t display;

SSD1306_t *init_display_device(void) {
    spi_master_init(&display, PIN_NUM_MOSI, PIN_NUM_CLK, -1, PIN_NUM_DC,
                    PIN_NUM_RST);
    // The original size is 128x64
    ssd1306_init(&display, 128, 32); // Lower number to increase font size

    // Clear the screen
    ssd1306_clear_screen(&display, false);
    return &display;
}

void app_main(void) {
    ESP_LOGI("MAIN", "Initializing NVS");
    nvs_handle_t handle = init_nvs_handle();

    ESP_LOGI("MAIN", "Initializing SSD1306 Display");
    SSD1306_t *display = init_display_device();

    ssd1306_display_text(display, 0, "I'am Thorkild!  ", 16, false);
    ssd1306_display_text(display, 1, "Enter code to,  ", 16, false);
    ssd1306_display_text(display, 2, "Figure It Out   ", 16, false);
    ssd1306_display_text(display, 3, "with me\\ (^_^) /", 16, false);

    ESP_LOGI("MAIN", "STARTING DUAL ADVERTISEMENT");
    start_dual_advertising();

    ESP_LOGI("MAIN", "Starting Keypad");
    keypad_main(handle, display);
}
