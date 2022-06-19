#include "led_handler.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "led_strip.h"
#include "string.h"

#define NBR_OF_LEDS 30
#define LED_GPIO GPIO_NUM_12
#define LED_GPIO_BLUE GPIO_NUM_1
#define LED_GPIO_RED GPIO_NUM_2

#define RMT_TX_CHANNEL RMT_CHANNEL_0


static led_strip_t* strip;
static color_rgb_t last_color;
bool is_init = false;

static const char* TAG = "LEDS";


void led_set_red(int on)
{
    gpio_set_level(LED_GPIO_RED, on);
}

void led_set_blue(int on)
{
    gpio_set_level(LED_GPIO_BLUE, on);
}

void led_init()
{
    if (is_init)
        return;

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(LED_GPIO, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(NBR_OF_LEDS, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        ESP_LOGE(TAG, "install WS2812 driver failed");
        return;
    }

    gpio_reset_pin(LED_GPIO_BLUE);
    gpio_set_direction(LED_GPIO_BLUE, GPIO_MODE_OUTPUT);
    led_set_blue(0);

    gpio_reset_pin(LED_GPIO_RED);
    gpio_set_direction(LED_GPIO_RED, GPIO_MODE_OUTPUT);
    led_set_red(0);

    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
    // Show simple rainbow chasing pattern
    ESP_LOGI(TAG, "LED handler started");
    is_init = true;
}

void led_set_color(const color_rgb_t* color)
{
    if (!is_init) {
        ESP_LOGE(TAG, "Led drier not started!");
        return;
    }

    for (int i = 0; i < NBR_OF_LEDS; i++) {
        strip->set_pixel(strip, i, color->r, color->b, color->g);
        strip->refresh(strip, 100);
    }

    memcpy(&last_color, color, sizeof(color_rgb_t));
}


void led_off()
{
    if (!is_init) {
        ESP_LOGE(TAG, "Led drier not started!");
        return;
    }
    strip->clear(strip, 100);
}

void led_on()
{
    if (!is_init) {
        ESP_LOGE(TAG, "Led drier not started!");
        return;
    }
    led_set_color(&last_color);
}
