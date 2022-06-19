#include "tradfri.h"
#include "cJSON.h"

#include "esp_log.h"
#include "esp_system.h"
#include "led_handler.h"
#include "string.h"

static tradfri_t tradfri;

#define TRADFRI_COAP_LIGHT_BULB "3311"
#define TRADFRI_COAP_LIGHT_BULB_IS_ON "5850"
#define TRADFRI_COAP_LIGHT_BULB_BRIGHTNESS "5851"
#define TRADFRI_COAP_LIGHT_BULB_COLOR "5706"


static const char* COLOR_1 = "efd275";
static const char* COLOR_2 = "f1e0b5";
static const char* COLOR_3 = "f2eccf";
static const char* COLOR_4 = "f3f3e3";
static const char* COLOR_5 = "f5faf6";

static const char* COLOR_1_REPLACEMENT = "aa4400";
static const char* COLOR_2_REPLACEMENT = "ff6611";
static const char* COLOR_3_REPLACEMENT = "ff6622";
static const char* COLOR_4_REPLACEMENT = "ffaa44";
static const char* COLOR_5_REPLACEMENT = "ffff99";

static const char* TAG = "Tradfri";

void tradfri_init()
{

}

static int hex_to_dec(const char hex[2])
{
    int msb = (hex[0] >= 'a') ? 10 + hex[0] - 'a' : hex[0] - '0';
    int lsb = (hex[1] >= 'a') ? 10 + hex[1] - 'a' : hex[1] - '0';
    return 16*msb + lsb;
}

static void update_led_strip()
{
    if (tradfri.is_on) {
        led_set_color(&tradfri.color);
    } else {
        led_off();
    }
}

static void get_color(char led_color[6], const char tradfri_color[6])
{
    if (strncmp(COLOR_1, tradfri_color, 6) == 0) {
        memcpy(&led_color, COLOR_1_REPLACEMENT, 6);
    } else if (strncmp(COLOR_2, tradfri_color, 6) == 0) {
        memcpy(&led_color, COLOR_2_REPLACEMENT, 6);
    } else if (strncmp(COLOR_3, tradfri_color, 6) == 0) {
        memcpy(&led_color, COLOR_3_REPLACEMENT, 6);
    } else if (strncmp(COLOR_4, tradfri_color, 6) == 0) {
        memcpy(&led_color, COLOR_4_REPLACEMENT, 6);
    } else if (strncmp(COLOR_5, tradfri_color, 6) == 0) {
        memcpy(&led_color, COLOR_5_REPLACEMENT, 6);
    }
}

static void adjust_for_brightness(color_rgb_t* color, const uint8_t brightness)
{
    float brightness_perc = brightness == 1 ? 0.1 : (float) brightness / 254.0;
    color->r *= brightness_perc;
    color->g *= brightness_perc;
    color->b *= brightness_perc;
}

void parse_coap_response(const char* data, const int data_len)
{
    cJSON* json = cJSON_ParseWithLength((const char*) data, data_len);
    if(json == NULL) {
        ESP_LOGW(TAG, "JSON parsing fail.\n");
        goto end;
    }

    cJSON* bulbs = cJSON_GetObjectItem(json, TRADFRI_COAP_LIGHT_BULB);

    if(bulbs == NULL) {
        ESP_LOGW(TAG, "BULBS parsing fail.\n");
        goto end;
    }

    int bulb_arr_size = cJSON_GetArraySize(bulbs);

    for (int i = 0; i < bulb_arr_size; i++) {
        cJSON* bulb = cJSON_GetArrayItem(bulbs, i);
        if(bulb == NULL) {
            ESP_LOGW(TAG, "BULB parsing fail.\n");
            goto end;
        }

        cJSON* is_on = cJSON_GetObjectItem(bulb, TRADFRI_COAP_LIGHT_BULB_IS_ON);
        if(is_on == NULL) {
            ESP_LOGW(TAG, "IS ON parsing fail.\n");
            goto end;
        }
        cJSON* brightness = cJSON_GetObjectItem(bulb, TRADFRI_COAP_LIGHT_BULB_BRIGHTNESS);
        if(brightness == NULL) {
            ESP_LOGW(TAG, "IS ON parsing fail.\n");
            goto end;
        }
        cJSON* color = cJSON_GetObjectItem(bulb, TRADFRI_COAP_LIGHT_BULB_COLOR);
        if(color == NULL) {
            ESP_LOGW(TAG, "IS ON parsing fail.\n");
            goto end;
        }

        tradfri.is_on = is_on->valueint;
        tradfri.brightness = brightness->valueint;

        char led_color[6];
        get_color(led_color, color->valuestring);
        tradfri.color.r = hex_to_dec(led_color);
        tradfri.color.g = hex_to_dec(&led_color[2]);
        tradfri.color.b = hex_to_dec(&led_color[4]);

        adjust_for_brightness(&tradfri.color, tradfri.brightness);

        ESP_LOGI(TAG, "on: %d, brightness: %d, color: %d, %d, %d", tradfri.is_on, tradfri.brightness, tradfri.color.r, tradfri.color.g, tradfri.color.b);

    }

    end:
        cJSON_Delete(json);   // Must free memory after used.
        ESP_LOGI(TAG, "Heap: %d", esp_get_free_heap_size());
        update_led_strip();
}
