#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "ultrasonic.h"
#include "driver/dac.h"

#define MAX_DISTANCE_CM 450 // 5m max


#define GPIO_TRIGGER	22
#define GPIO_ECHO	23

void ultrasonic(void *pvParamters)
{
	ultrasonic_sensor_t sensor = {
		.trigger_pin = GPIO_TRIGGER,
		.echo_pin = GPIO_ECHO
	};

	ultrasonic_init(&sensor);

	while (true) {
		uint32_t distance;
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
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}


void app_main()
{

	xTaskCreate(ultrasonic, "ultrasonic", 2048, NULL, 5, NULL);
}

