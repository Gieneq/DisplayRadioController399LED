#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "../gtypes.h"

#define LED_MATRIX_COLUMNS      (19)
#define LED_MATRIX_ROWS         (21)
#define LED_MATRIX_PIXELS_COUNT (LED_MATRIX_COLUMNS * LED_MATRIX_ROWS)

#define LED_MATRIX_HAS_COLOR    (1<<0)

typedef struct led_matrix_t {
    color_24b_t pixels[LED_MATRIX_PIXELS_COUNT];
    uint16_t columns;
    uint16_t rows;
    uint16_t pixels_count;
    uint8_t flags;
} led_matrix_t;

void led_matrix_init(led_matrix_t* led_matrix);

void led_matrix_clear(led_matrix_t* led_matrix);

void led_matrix_fill_color(led_matrix_t* led_matrix, color_24b_t color);

color_24b_t* led_matrix_access_pixel_at(led_matrix_t* led_matrix, uint16_t x, uint16_t y);

#ifdef __cplusplus
}
#endif