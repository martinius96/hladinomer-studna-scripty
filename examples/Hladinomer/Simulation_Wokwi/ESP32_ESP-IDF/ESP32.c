/*|-----------------------------------------------------------------------------------|*/
/*|Project: Water Level Monitor - HTTPS - ESP32 + HC-SR04                             |*/
/*|Framework: PURE ESP-IDF v5.3+, tested in Wokwi on v5.3.4                           |*/
/*|Project info: https://your-iot.github.io/Watmonitor/                               |*/
/*|Watmonitor interface: https://hladinomer.eu/?lang=en                               |*/
/*|-----------------------------------------------------------------------------------|*/

//Project: https://wokwi.com/projects/459157676335508481

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "esp_crt_bundle.h"

#include "ultrasonic.h"

/* --- KONFIGURÁCIA --- */
#define WIFI_SSID      "Wokwi-GUEST"
#define WIFI_PASS      ""
#define MAX_RETRY      5

#define MAX_DISTANCE_CM 450
#define GPIO_TRIGGER    22
#define GPIO_ECHO       23

#define WEB_SERVER      "hladinomer.eu"
#define WEB_PORT        "443"

static const char *TAG_WIFI = "WIFI_STA";

static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
static int s_retry_num = 0;

QueueHandle_t q = NULL;

/* Wi-Fi Event Handler */
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG_WIFI, "Retrying connection to AP...");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* Wi-Fi Initialization */
void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}

/* Ultrasonic Task */
static void ultrasonic_task(void *pvParamters)
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = GPIO_TRIGGER,
        .echo_pin = GPIO_ECHO
    };
    ultrasonic_init(&sensor);
    uint32_t distance = 0;

    while (true) {
        uint32_t avg_distance = 0;
        int successful_reads = 0;

        for (int i = 0; i < 10; i++) {
            esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance);
            if (res == ESP_OK) {
                avg_distance += distance;
                successful_reads++;
                printf("Sample %d: %" PRIu32 " cm\n", successful_reads, distance);
            }
            vTaskDelay(pdMS_TO_TICKS(60));
        }

        if (successful_reads > 0) {
            uint32_t final_val = avg_distance / successful_reads;
            xQueueSend(q, &final_val, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(300000)); // 5 minút
    }
}

/* HTTPS Task */
static void https_post_task(void *pvParameters)
{
    uint32_t distance_to_send;
    char buf[512];
    int ret, len;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

    mbedtls_ssl_init(&ssl);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);

    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
    esp_crt_bundle_attach(&conf);

    mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_setup(&ssl, &conf);

    while (1) {
        if (xQueueReceive(q, &distance_to_send, portMAX_DELAY)) {
            char values[128];
            char REQUEST[512];
            snprintf(values, sizeof(values), "hodnota=%" PRIu32 "&token=123456789", distance_to_send);
            snprintf(REQUEST, sizeof(REQUEST),
                     "POST /data.php HTTP/1.1\r\n"
                     "Host: " WEB_SERVER "\r\n"
                     "User-Agent: ESP32-IDF\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "Content-Length: %d\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "%s", (int)strlen(values), values);

            mbedtls_net_init(&server_fd);
            if ((ret = mbedtls_net_connect(&server_fd, WEB_SERVER, WEB_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
                goto cleanup;
            }

            mbedtls_ssl_set_hostname(&ssl, WEB_SERVER);
            mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

            while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
                if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) goto cleanup;
            }

            mbedtls_ssl_write(&ssl, (const unsigned char *)REQUEST, strlen(REQUEST));
            
            do {
                len = sizeof(buf) - 1;
                bzero(buf, sizeof(buf));
                ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf, len);
                if (ret > 0) printf("%s", buf);
            } while (ret > 0);

        cleanup:
            mbedtls_ssl_session_reset(&ssl);
            mbedtls_net_free(&server_fd);
        }
    }
}

/* --- HLAVNÝ VSTUP --- */
void app_main() 
{
  // Výpis verzie ESP-IDF
    printf("\n--- INFO ---\n");
    printf("ESP-IDF Version: %s\n", esp_get_idf_version());
    printf("-----------\n");
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("CPU cores: %d\n", chip_info.cores);
    printf("Revision: %d\n", chip_info.revision);
    printf("Free heap: %lu\n", esp_get_free_heap_size());
    printf("Set distance via HC-SR04 sensor, you have 5 seconds\n");
    vTaskDelay(pdMS_TO_TICKS(5000));
    // Inicializácia Flash pamäte
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Pripojenie Wi-Fi
    wifi_init_sta();

    // Vytvorenie fronty a úloh
    q = xQueueCreate(10, sizeof(uint32_t));
    if (q != NULL) {
        xTaskCreate(ultrasonic_task, "ultrasonic", 4096, NULL, 5, NULL);
        xTaskCreate(https_post_task, "https_post", 8192, NULL, 5, NULL);
    }

    // Hlavná slučka (voliteľné, app_main nesmie skončiť hneď ak nechceš reštart)
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
