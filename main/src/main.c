#include "wifi_handler.h"
#include "coap_handler.h"
#include "led_handler.h"


void app_main(void)
{
    led_init();
    led_set_red(1);
    wifi_init();
    coap_init();
    led_set_blue(1);
    led_set_red(0);
}
