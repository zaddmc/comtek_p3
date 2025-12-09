#include "common.h"
#include "display_handler.h"
#include "ssd1306.h"
#include "string.h"

static SSD1306_t DISPLAY;

void init_display_device(void) {
    spi_master_init(&DISPLAY, DISP_PIN_NUM_MOSI, DISP_PIN_NUM_CLK, -1,
                    DISP_PIN_NUM_DC, DISP_PIN_NUM_RST);

    // The original size is 128x64
    ssd1306_init(&DISPLAY, 128, 32); // Lower number to increase font size

    update_display(0);
    return;
}

void update_display(screen_state_t state) {
    ssd1306_clear_screen(&DISPLAY, false);
    switch (state) {
    case WELCOME_MSG:
        ssd1306_display_text(&DISPLAY, 0, "I am Thorkild!", 14, false);
        ssd1306_display_text(&DISPLAY, 1, "Enter code to,", 14, false);
        ssd1306_display_text(&DISPLAY, 2, "Figure-It-Out", 13, false);
        ssd1306_display_text(&DISPLAY, 3, "with me\\ (^_^) /", 16, false);
        break;
    case KEYPAD_UNLOCK:
        if (is_unlocked)
            ssd1306_display_text(&DISPLAY, 0, "Unlocked", 8, false);
        else
            ssd1306_display_text(&DISPLAY, 0, "Locked", 6, false);
        ssd1306_display_text(&DISPLAY, 1, "Current code", 12, false);
        ssd1306_display_text(&DISPLAY, 2, keypad_input, strlen(keypad_input),
                             false);
        ssd1306_display_text(&DISPLAY, 3, "   \\ (^_^) /", 13, false);
        break;
    case LACKING_TICKS:
        ssd1306_display_text(&DISPLAY, 0, "Sorry", 5, false);
        ssd1306_display_text(&DISPLAY, 1, "No more ticks", 13, false);
        ssd1306_display_text(&DISPLAY, 2, "Contact renters", 15, false);
        ssd1306_display_text(&DISPLAY, 3, "Place for code", 14, false);
        break;
    case DEBUG:
        ssd1306_display_text(&DISPLAY, 0, "THIS DEBUG ONLY!", 16, false);
        ssd1306_display_text(&DISPLAY, 1, "Added 5 ticks", 13, false);
        ssd1306_display_text(&DISPLAY, 3, "REMOVE THIS CODE", 16, false);
        break;
    case BLE_UNLOCK:
        ssd1306_display_text(&DISPLAY, 1, "    Unlocked!", 13, false);
        ssd1306_display_text(&DISPLAY, 2, "    \\ (^_^) /", 13, false);
        break;
    default:
        ssd1306_display_text(&DISPLAY, 0, "Invalid state", 13, false);
        break;
    }
}
