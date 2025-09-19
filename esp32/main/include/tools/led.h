#ifndef LED
#define LED

#include "led_strip.h"
#include "led_strip_types.h"

void led_init(void);

void led_on(uint32_t red, uint32_t green, uint32_t blue);

void led_off();

#endif
