#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "driver/adc.h"
#include "driver/gpio.h"

#define WIFI_SSID ""     
#define WIFI_PASS ""     
#define MQTT_URI  "" 

#define VRX_ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34 - Horizontal
#define VRY_ADC_CHANNEL ADC1_CHANNEL_7 // GPIO35 - Vertical
#define DEADZONE 300
#define NUM_SAMPLES 5

static esp_mqtt_client_handle_t client;
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        printf("Reconectando ao Wi-Fi...\n");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("Conectado. IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        }
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        printf("MQTT conectado!\n");
        break;
    case MQTT_EVENT_DISCONNECTED:
        printf("MQTT desconectado!\n");
        break;
    default:
        break;
    }
}

int read_average(adc1_channel_t channel)
{
    int total = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        total += adc1_get_raw(channel);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return total / NUM_SAMPLES;
}

static void joystick_task(void *pvParameter)
{
    int center_x = read_average(VRX_ADC_CHANNEL);
    int center_y = read_average(VRY_ADC_CHANNEL);

    const char *current_direction = NULL;
    const char *last_direction = NULL;

    TickType_t last_publish_time = 0;
    const TickType_t repeat_delay = pdMS_TO_TICKS(500);

    while (1)
    {
        int x = read_average(VRX_ADC_CHANNEL);
        int y = read_average(VRY_ADC_CHANNEL);
        current_direction = NULL;

        if (x > center_x + DEADZONE)
            current_direction = "right";
        else if (x < center_x - DEADZONE)
            current_direction = "left";
        else if (y > center_y + DEADZONE)
            current_direction = "up";
        else if (y < center_y - DEADZONE)
            current_direction = "down";

        TickType_t now = xTaskGetTickCount();

        if (current_direction != NULL)
        {
            if (last_direction == NULL || strcmp(current_direction, last_direction) != 0 || (now - last_publish_time >= repeat_delay))
            {
                esp_mqtt_client_publish(client, "/ifpe/ads/joystick", current_direction, 0, 1, 0);
                printf("Direção: %s\n", current_direction);
                last_direction = current_direction;
                last_publish_time = now;
            }
        }
        else
        {
            last_direction = NULL;
        }

        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}


void app_main(void)
{
    nvs_flash_init();
    wifi_init_sta();

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_URI,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    esp_netif_ip_info_t ip;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_get_ip_info(netif, &ip);
    printf("ESP32 IP: " IPSTR "\n", IP2STR(&ip.ip));

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(VRX_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(VRY_ADC_CHANNEL, ADC_ATTEN_DB_11);

    xTaskCreate(joystick_task, "joystick_task", 4096, NULL, 5, NULL);
}
