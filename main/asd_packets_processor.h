#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "gtypes.h"
#include "leds/led_matrix.h"
#include "rf/asd_packet.h"
#include "freertos/FreeRTOS.h"

void asd_packets_processor_init();

void asd_packets_processor_push_packet(const asd_packet_t* packet);

const led_matrix_t* asd_packets_processor_poll_completed_led_matrix();

const led_matrix_t* asd_packets_processor_wait_for_completed_led_matrix(TickType_t ticks_to_wait);

#ifdef __cplusplus
}
#endif