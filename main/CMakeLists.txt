idf_component_register(SRCS "main.c" "leds/led_matrix.c" "leds/ws2812b_grid.c" "rf/rf_receiver.c"
                    PRIV_REQUIRES nvs_flash esp_event esp_netif esp_wifi esp_timer 
                    INCLUDE_DIRS "")
