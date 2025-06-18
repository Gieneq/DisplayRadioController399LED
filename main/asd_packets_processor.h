#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "gtypes.h"
#include "leds/led_matrix.h"
#include "rf/asd_packet.h"

// typedef void (*asd_packets_processor_on_complete_led_matrix_t)(const led_matrix_t*);

// esp_err_t asd_packets_processor_init(asd_packets_processor_on_complete_led_matrix_t callback);
void asd_packets_processor_init();

void asd_packets_processor_push_packet(const asd_packet_t* packet);

const led_matrix_t* asd_packets_processor_poll_completed_led_matrix();

#ifdef __cplusplus
}
#endif