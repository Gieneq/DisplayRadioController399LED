#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "nvs_flash.h"

#include "esp_log.h"
#include "esp_heap_caps.h"

#include "gtypes.h"
#include "leds/ws2812b_grid.h"
#include "rf/rf_receiver.h"
#include "asd_packets_processor.h"

static const char TAG[] = "Main";

static led_matrix_t* workspace_led_matrix;

static void info_prints() {
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}

void on_asd_packet_received_callback(const asd_packet_t* packet) {
    asd_packets_processor_push_packet(packet);
}

void app_main(void) {
    esp_err_t ret = ESP_OK;
    info_prints();
    ESP_LOGI(TAG, "Starting!");

    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    workspace_led_matrix = heap_caps_calloc(1, sizeof(led_matrix_t ), MALLOC_CAP_DEFAULT);
    assert(workspace_led_matrix);
    
    ret = ws2812b_grid_init();
    ESP_ERROR_CHECK(ret);

    ret = rf_receiver_init(on_asd_packet_received_callback);
    ESP_ERROR_CHECK(ret);

    asd_packets_processor_init();

    ws2812b_grid_interface_t* grid_if = NULL;

    while(1) {
        if (ws2812b_grid_access(&grid_if, portMAX_DELAY)) {
            grid_if->set_led_matrix_values(workspace_led_matrix);
            ws2812b_grid_release();
            grid_if = NULL;
        }

        const led_matrix_t* received_led_matrix = asd_packets_processor_wait_for_completed_led_matrix(portMAX_DELAY);
        if (received_led_matrix != NULL) {
            memcpy(workspace_led_matrix, received_led_matrix, sizeof(led_matrix_t));
        }
    }
}
