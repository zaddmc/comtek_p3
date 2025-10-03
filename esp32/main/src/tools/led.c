#include "tools/led.h"
#include "commmon.h"
#include "led_strip.h"
#include <stdint.h>

static led_strip_handle_t led_strip;
void led_init(void) {
  ESP_LOGI(TAG, "example configured to blink addressable led!");
  /* LED strip initialization with the GPIO and pixels number*/
  led_strip_config_t strip_config = {
      .strip_gpio_num = 8,
      .max_leds = 1, // at least one LED on board
  };
  led_strip_rmt_config_t rmt_config = {
      .resolution_hz = 10 * 1000 * 1000, // 10MHz
      .flags.with_dma = false,
  };
  ESP_ERROR_CHECK(
      led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
}

void led_on(uint32_t red, uint32_t green, uint32_t blue) {
  led_strip_set_pixel(led_strip, 0, red, green, blue);
  led_strip_refresh(led_strip);
}

void led_off() {
  led_strip_clear(led_strip);
  led_strip_refresh(led_strip);
}
