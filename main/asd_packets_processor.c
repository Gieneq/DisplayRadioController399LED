#include "asd_packets_processor.h"

#include <string.h>
#include "freertos/event_groups.h"
#include "esp_log.h"

#define ASD_PACKET_EVENT_NEW_MATRIX_BIT BIT0

static const char *TAG = "ASD_PACKETS_PROC";

typedef struct asd_packets_processor_t {
    led_matrix_t acc_led_matrix_data;
    led_matrix_t led_matrix_completed;
    EventGroupHandle_t event_group;
} asd_packets_processor_t;

static asd_packets_processor_t asd_packets_processor;

void asd_packets_processor_init() {
    led_matrix_clear(&asd_packets_processor.acc_led_matrix_data);
    led_matrix_clear(&asd_packets_processor.led_matrix_completed);
    asd_packets_processor.event_group = xEventGroupCreate();
}

void asd_packets_processor_push_packet(const asd_packet_t* packet) {
    uint8_t* accumulator_beginning = (uint8_t*)(&asd_packets_processor.acc_led_matrix_data);
    const uint32_t data_offset = packet->packet_idx * ASD_PACKET_DATA_SIZE;

    memcpy(
        accumulator_beginning + data_offset ,
        packet->data,
        packet->data_size
    );

    if (packet->packet_idx == (packet->total_packets_count - 1)) {
        ESP_LOGV(TAG, "Finish collecting led_matrix data, rows=%u, cols=%u",
            asd_packets_processor.acc_led_matrix_data.rows,
            asd_packets_processor.acc_led_matrix_data.columns
        );
            
        for (uint16_t idx_y = 0; idx_y < LED_MATRIX_ROWS; ++idx_y) {
            for (uint16_t idx_x = 0; idx_x < LED_MATRIX_COLUMNS; ++idx_x) {
                const color_24b_t color = *led_matrix_access_pixel_at(&asd_packets_processor.acc_led_matrix_data, idx_x, idx_y);
                if (color.value != 0) {
                    ESP_LOGV(TAG, "Color at [%u, %u] is (%u, %u, %u)",
                        idx_x,
                        idx_y,
                        color.red, color.green, color.blue
                    );
                }
            }
        }

        memcpy(&asd_packets_processor.led_matrix_completed, &asd_packets_processor.acc_led_matrix_data, sizeof(led_matrix_t));
        xEventGroupSetBits(asd_packets_processor.event_group, ASD_PACKET_EVENT_NEW_MATRIX_BIT);
    }
}

const led_matrix_t* asd_packets_processor_wait_for_completed_led_matrix(TickType_t ticks_to_wait) {
    if (asd_packets_processor.event_group == NULL) {
        return NULL;
    }

    EventBits_t bits = xEventGroupWaitBits(
        asd_packets_processor.event_group,
        ASD_PACKET_EVENT_NEW_MATRIX_BIT,
        pdTRUE,     // Clear on exit
        pdFALSE,    // Wait for any bit (only one defined)
        ticks_to_wait
    );

    if (bits & ASD_PACKET_EVENT_NEW_MATRIX_BIT) {
        return &asd_packets_processor.led_matrix_completed;
    }

    return NULL;
}
