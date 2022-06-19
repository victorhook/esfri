#pragma once

#include "stdint.h"
#include "tradfri.h"

#define WS2812_T0H_NS (500)
#define WS2812_T0L_NS (2000)
#define WS2812_T1H_NS (2000)
#define WS2812_T1L_NS (500)
#define WS2812_RESET_US (2500)

void led_init();

void led_set_color(const color_rgb_t* color);

void led_off();

void led_on();

void led_set_red(int on);

void led_set_blue(int on);
