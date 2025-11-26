#include "display_handler.h"
#include "esp_log.h"
#include "ssd1306.h"

#define PIN_NUM_CLK 36  // SCK
#define PIN_NUM_MOSI 35 // SDA
#define PIN_NUM_RST 47  // RES
#define PIN_NUM_DC 20   // DC

static SSD1306_t DISPLAY;

void init_display_device(void) {

    spi_master_init(&DISPLAY, PIN_NUM_MOSI, PIN_NUM_CLK, -1, PIN_NUM_DC,
                    PIN_NUM_RST);

    // The original size is 128x64
    ssd1306_init(&DISPLAY, 128, 32); // Lower number to increase font size

    update_display(0);
    return;
}

void update_display(int state) {
    ssd1306_clear_screen(&DISPLAY, false);
    switch (state) {
    case 0:
        ssd1306_display_text(&DISPLAY, 0, "I am Thorkild!", 14, false);
        ssd1306_display_text(&DISPLAY, 1, "Enter code to,", 14, false);
        ssd1306_display_text(&DISPLAY, 2, "Figure-It-Out", 13, false);
        ssd1306_display_text(&DISPLAY, 3, "with me\\ (^_^) /", 16, false);
        break;
    case 1:
        if (is_unlocked)
            ssd1306_display_text(&DISPLAY, 0, "Unlocked", 8, false);
        else
            ssd1306_display_text(&DISPLAY, 0, "Locked", 6, false);
        ssd1306_display_text(&DISPLAY, 1, "Current code", 12, false);
        ssd1306_display_text(&DISPLAY, 2, keypad_input, strlen(keypad_input),
                             false);
        ssd1306_display_text(&DISPLAY, 3, "   \\ (^_^) /", 13, false);
        break;
    case -1:
        ssd1306_display_text(&DISPLAY, 0, "Sorry", 5, false);
        ssd1306_display_text(&DISPLAY, 1, "No more ticks", 13, false);
        ssd1306_display_text(&DISPLAY, 2, "Contact renters", 15, false);
        ssd1306_display_text(&DISPLAY, 3, "Place for code", 14, false);
        break;
    case 2:
        ssd1306_display_text(&DISPLAY, 0, "THIS DEBUG ONLY!", 16, false);
        ssd1306_display_text(&DISPLAY, 1, "Added 5 ticks", 13, false);
        ssd1306_display_text(&DISPLAY, 3, "REMOVE THIS CODE", 16, false);
        break;
    default:
        ssd1306_display_text(&DISPLAY, 0, "Invalid state", 13, false);
        break;
    }
}
