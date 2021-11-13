#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

extern "C" {
    #include "button.h"
}

#include "timer.h"
#include "protocol_examples_common.h"
#include "websocket_client.h"
#include "i2s.h"

#define TAG "MAIN"

enum STATES {
    Idle,
    Listening,
    Speaking
};
STATES states = Idle;

const char * websockets_server_host = "185.149.40.141";
// const char *websockets_server_host = "192.168.43.26";
const uint16_t websockets_server_port = 9999;

I2S::I2S_TX i2s_tx;
I2S::I2S_RX i2s_rx;

#define RIGHT_BUTTON_PIN 35

websocket_client socket;

TaskHandle_t i2sReadTaskHandler = NULL;

button_event_t ev;
QueueHandle_t button_events = button_init(PIN_BIT(RIGHT_BUTTON_PIN));

static void button_task(void* pParams) {
    while (true) {
        if (xQueueReceive(button_events, &ev, 1000 / portTICK_PERIOD_MS)) {
            if (ev.pin == RIGHT_BUTTON_PIN) {
            }
        }
    }
}

static void ping_task(void* arg) {
}

void i2s_read_task(void * arg) {
}

void actionCommand(unsigned long timestamp) {
    if (timeStampOffset == 0) {
        unsigned long currentTimestamp = millis();
        timeStampOffset = currentTimestamp >= timestamp ? currentTimestamp - timestamp : timestamp - currentTimestamp;
        return;
    }
}

static void websocket_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_websocket_event_data_t* data = (esp_websocket_event_data_t*)event_data;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
            ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
            ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", 
                        data->payload_len, data->data_len, data->payload_offset);
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
            break;
    }
}

static void websocket_app_start(void) {
}

void init_button(void) {
}