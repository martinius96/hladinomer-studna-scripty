/*|-----------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTP - HC-SR04 / JSN-SR04T / HY-SRF05                        |*/
/*|ESP32 (DevKit, Generic) - ESP-IDF v4.2 (4.0 compatible)                            |*/
/*|Autor: Martin Chlebovec (martinius96)                                              |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/ |*/
/*|Testovacie webove rozhranie: http://arduino.clanweb.eu/studna_s_prekladom/         |*/
/*|Revízia: 4. Jun 2021                                                               |*/
/*|-----------------------------------------------------------------------------------|*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "ultrasonic.h"
#include "driver/dac.h"

/* Constants that aren't configurable in menuconfig */

#define MAX_DISTANCE_CM 450 // 5m max
#define GPIO_TRIGGER	22
#define GPIO_ECHO	23
// Webserver
#define WEB_SERVER "arduino.clanweb.eu"
#define WEB_PORT "80"


static const char *TAG = "example";

QueueHandle_t  q=NULL;
static void ultrasonic(void *pvParamters)
{
	ultrasonic_sensor_t sensor = {
		.trigger_pin = GPIO_TRIGGER,
		.echo_pin = GPIO_ECHO
	};

	ultrasonic_init(&sensor);
uint32_t distance = 0;
    if(q == NULL){
        printf("Queue is not ready \n");
        return;
    }
	while (true) {
		esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance);
		if (res != ESP_OK) {
			printf("Error: ");
			switch (res) {
				case ESP_ERR_ULTRASONIC_PING:
					printf("Cannot ping (device is in invalid state)\n");
					break;
				case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
					printf("Ping timeout (no device found)\n");
					break;
				case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
					printf("Echo timeout (i.e. distance too big)\n");
					break;
				default:
					printf("%d\n", res);
			}
		} else {
			printf("Distance: %d cm, %.02f m\n", distance, distance / 100.0);
		}
    xQueueSend(q,(void *)&distance,(TickType_t )0); // add the counter value to the queue
            for(int countdown = 300; countdown >= 0; countdown--) {
            ESP_LOGI(TAG, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
	}
}

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];
    uint32_t distance;
     if(q == NULL){
        printf("Queue is not ready \n");
        return;
    }
    while(1) {
    xQueueReceive(q,&distance,(TickType_t )(350000/portTICK_PERIOD_MS)); 
    char REQUEST [1000];
	char values [250];
	sprintf(values, "hodnota=%d&token=123456789", distance);
  sprintf (REQUEST, "POST /studna_s_prekladom/data.php HTTP/1.0\r\nHost: "WEB_SERVER":"WEB_PORT"\r\nUser-Agent: ESP32\r\nConnection: close\r\nContent-Type: application/x-www-form-urlencoded;\r\nContent-Length:%d\r\n\r\n%s\r\n",strlen(values),values);
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        close(s);
        ESP_LOGI(TAG, "Starting again!");
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
     q=xQueueCreate(20,sizeof(unsigned long));
    if(q != NULL){
        printf("Queue is created\n");
        vTaskDelay(1000/portTICK_PERIOD_MS); //wait for a second
        xTaskCreate(&ultrasonic, "ultrasonic", 2048, NULL, 5, NULL);
        printf("producer task  started\n");
        xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
        printf("consumer task  started\n");
    }else{
        printf("Queue creation failed");
    }    

}
