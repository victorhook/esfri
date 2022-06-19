#pragma once

#include "stdint.h"


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed)) color_rgb_t;

typedef struct {
    uint8_t is_on;
    color_rgb_t color;
    uint8_t brightness;
} tradfri_t;


void tradfri_init();

void parse_coap_response(const char* data, const int data_len);
