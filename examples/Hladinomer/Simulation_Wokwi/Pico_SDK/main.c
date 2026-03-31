/*|------------------------------------------------------------|*/
/*|Project: Water Level Monitor - HTTPS - Rpi Pico W + HC-SR04 |*/
/*|SDK: Pico SDK 2.1.1                                         |*/
/*|Project info: https://your-iot.github.io/Watmonitor/        |*/
/*|Watmonitor interface: https://hladinomer.eu/?lang=en        |*/
/*|------------------------------------------------------------|*/

// Simulate at: https://wokwi.com/projects/460018702245115905

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

// For SDK and board info
#include "pico/version.h"
#include "boards/pico_w.h"

// --- CONFIGURATION ---
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""
#define SERVER_HOST "hladinomer.eu"
#define SERVER_PORT 80
#define URL_PATH "/data.php"
#define TOKEN "123456789"

#define TRIG_PIN 3
#define ECHO_PIN 2

// Global variables for asynchronous HTTP
static ip_addr_t server_ip;
static bool dns_found = false;
static bool request_sent = false;
static float current_distance = 0.0f;

// --- HC-SR04 DISTANCE MEASUREMENT ---
float measure_distance() {
    gpio_put(TRIG_PIN, 0);
    sleep_us(2);
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    // Wait for the pulse to start
    while (gpio_get(ECHO_PIN) == 0) {
        tight_loop_contents();
    }
    absolute_time_t start_time = get_absolute_time();

    // Wait for the pulse to end
    while (gpio_get(ECHO_PIN) == 1) {
        tight_loop_contents();
    }
    absolute_time_t end_time = get_absolute_time();

    int64_t pulse_duration = absolute_time_diff_us(start_time, end_time);
    
    // Speed of sound is ~343 m/s -> 0.0343 cm/us
    float distance = (pulse_duration * 0.0343f) / 2.0f;
    return distance;
}

// --- LWIP HTTP CLIENT ---
// Callback when response is received from the server
static err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p != NULL) {
        printf("Server Response: %.*s\n", p->len, (char *)p->payload);
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
    }
    tcp_close(tpcb);
    request_sent = true;
    return ERR_OK;
}

// Callback when TCP connection is successfully established
static err_t http_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("TCP connection failed!\n");
        return err;
    }

    char post_data[64];
    snprintf(post_data, sizeof(post_data), "hodnota=%.1f&token=%s", current_distance, TOKEN);

    char headers[512];
    snprintf(headers, sizeof(headers),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: PicoW-MCU/1.0\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n"
             "%s",
             URL_PATH, SERVER_HOST, (int)strlen(post_data), post_data);

    tcp_recv(tpcb, http_recv_callback);
    tcp_write(tpcb, headers, strlen(headers), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    
    printf("HTTP POST sent: %s\n", post_data);
    return ERR_OK;
}

// Callback for DNS resolution
static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr) {
        server_ip = *ipaddr;
        dns_found = true;
   //     printf("DNS resolved. IP: %s\n", ipaddr_ntoa(ipaddr));
    } else {
    //    printf("DNS resolution failed!\n");
    }
}

// Function to trigger data sending
void send_data(float distance) {
    current_distance = distance;
    dns_found = false;
    request_sent = false;

   // printf("Resolving DNS for %s...\n", SERVER_HOST);
    err_t err = dns_gethostbyname(SERVER_HOST, &server_ip, dns_callback, NULL);
    
    if (err == ERR_OK) {
        dns_found = true; // IP was already cached
    }

    // Wait for DNS (asynchronous)
    int timeout = 0;
    while (!dns_found && timeout < 50) {
        cyw43_arch_poll();
        sleep_ms(100);
        timeout++;
    }

    if (!dns_found) {
      //  printf("DNS timeout!\n");
        return;
    }

    // Create TCP connection
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
     //   printf("Failed to create PCB!\n");
        return;
    }

    tcp_connect(pcb, &server_ip, SERVER_PORT, http_connected_callback);

    // Wait for the request to complete
    timeout = 0;
    while (!request_sent && timeout < 100) {
        cyw43_arch_poll();
        sleep_ms(100);
        timeout++;
    }
}

// --- MAIN ---
int main() {
    stdio_init_all();
    
    // Initialize GPIOs
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    printf("Set distance on HC-SR04 sensor within 5 seconds...\n");
    sleep_ms(5000);

    // --- SYSTEM INFO LOGGING ---
    printf("\n----------------------------------------\n");
    printf("        SYSTEM INFO        \n");
    printf("----------------------------------------\n");
    printf("Pico SDK Version: %s\n", PICO_SDK_VERSION_STRING);
    printf("----------------------------------------\n\n");

    // Initialize Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi module initialization failed!\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi: %s...\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Wi-Fi connection failed!\n");
        return -1;
    }
    printf("Connected to Wi-Fi!\n");

    while (true) {
        float dist = measure_distance();
        printf("Measured distance: %.1f cm\n", dist);
        
        send_data(dist);

        // 30 seconds wait loop keeping Wi-Fi alive
        for (int i = 0; i < 30; i++) {
            cyw43_arch_poll(); 
            sleep_ms(1000);
        }
    }

    cyw43_arch_deinit();
    return 0;
}
