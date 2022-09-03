#include "wifi_handler.h"
#include "coap_handler.h"
#include "led_handler.h"


void app_main(void)
{
    led_init();
    led_set_red(1);
    wifi_init();
    coap_init();
    //color_rgb_t color = {.r=255, .g=255, .b=255};
    //led_set_color(&color);
    led_set_blue(1);
    led_set_red(0);
}
