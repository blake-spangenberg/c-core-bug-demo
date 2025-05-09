#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define PUBNUB_ONLY_PUBSUB 1
#define PUBNUB_THREADSAFE 1
#include "pubnub_config.h"
#include "pubnub_sync.h"

/*** User Config ***/

#define SSID ""
#define PASS ""
#define AUTH_MODE WIFI_AUTH_WPA2_PSK

#if sizeof(SSID) == 1 || sizeof(PASS) == 1
    #error "Must set SSID and PASS for example to work"
#endif

/*** End User Config ***/

static bool wifi_connected = false;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI("wifi", "Connecting...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_connected = false;
        ESP_LOGE("wifi", "Disconnected.");
        ESP_LOGI("wifi", "Connecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("wifi", "Connected. IP=" IPSTR, IP2STR(&event->ip_info.ip));
        wifi_connected = true;
    }
}

static void wifi_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PASS,
            .threshold.authmode = AUTH_MODE,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void wifi_block_until_connected(void) {
    while (!wifi_connected) {
        vTaskDelay(10);
    }
}

bool should_disconnect = false; // External

static void subscribe_task(void) {
    pubnub_t* pubnub = NULL;
    enum pubnub_res ret;
    const char* message;

    while (true) {
        if (pubnub) {
            pubnub_free(pubnub);
        }

        pubnub = pubnub_alloc();
        pubnub_init(pubnub, pub_key, sub_key);
        pubnub_set_user_id(pubnub, user_id);
        pubnub_set_auth(pubnub, auth_key);

        while (!should_disconnect) {
            pubnub_subscribe(pubnub, channels, NULL);
            ret = pubnub_await(pubnub);
            if (ret != PNR_OK) {
                break;
            }

            while ((message = pubnub_get(pubnub))) {
                handle_message(message);
            }
        }
        should_disconnect = false;
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    wifi_block_until_connected();
}
