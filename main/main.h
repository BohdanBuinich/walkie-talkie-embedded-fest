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
                if (ev.event == BUTTON_HELD && states == Idle) {
                    std::string value = getStrTimestamp();
                    socket.send_text(value.c_str(), value.size(), portMAX_DELAY);
                } else if (ev.event == BUTTON_UP && states == Speaking) {
                    states = Idle;
                    ESP_LOGI(TAG, "* IDLE Mode *");

                    // print_log_screen("* IDLE Mode *");
                    vTaskDelay(pdMS_TO_TICKS(100));
                    if (i2sReadTaskHandler != NULL) {
                        vTaskDelete(i2sReadTaskHandler);
                        i2sReadTaskHandler = NULL;
                    }
                    vTaskDelay(pdMS_TO_TICKS(100));

                    std::string value = getStrTimestamp();
                    socket.send_text(value.c_str(), value.size(), portMAX_DELAY);
                    i2s_rx.uninst();
                }
            }
        }
    }
}

static void ping_task(void* arg) {
    char dummy_data[1];
    while (1) {
        if (states == Idle && socket.is_connected()) {
            ESP_LOGI(TAG, "Send dummy");
            socket.send_bin((const char*)dummy_data, 1, portMAX_DELAY);
        }
        vTaskDelay(1000);
    }
}

void i2s_read_task(void * arg) {
    while (1) {
        i2s_rx.read_data();
        socket.send_bin((const char*)i2s_rx.flash_write_buff, i2s_rx.I2S_READ_LEN, portMAX_DELAY);
    }
}

void actionCommand(unsigned long timestamp) {
    if (timeStampOffset == 0) {
        unsigned long currentTimestamp = millis();
        timeStampOffset = currentTimestamp >= timestamp ? currentTimestamp - timestamp : timestamp - currentTimestamp;
        return;
    }
    printf("STATE=%d,requestTimestamp=%ld, timestamp=%ld\n", states, requestTimestamp, timestamp);
    if (requestTimestamp == timestamp && states == Idle && ev.event == BUTTON_HELD) {
        states = Speaking;
        // print_log_screen("* Speaking Mode *");
        ESP_LOGI(TAG, "* Speaking Mode *");
        i2s_rx.init();
        if (i2sReadTaskHandler == NULL) {
            xTaskCreate(i2s_read_task, "i2s_read_task", 4096, NULL, 1, &i2sReadTaskHandler);
        }
    } else if (ev.event != BUTTON_HELD){
        if (states == Idle) {
            states = Listening;
            i2s_tx.init();
            ESP_LOGI(TAG, "* Listening Mode *");
            //   print_log_screen("* Listening Mode *");
        } else if (states == Listening) {
            states = Idle;
            i2s_tx.uninst();
            ESP_LOGI(TAG, "* IDLE Mode *");
            //   print_log_screen("* IDLE Mode *");
        }
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
            if (data->op_code == WS_TRANSPORT_OPCODES_TEXT || data->op_code == WS_TRANSPORT_OPCODES_BINARY)
            {
                if (states == Listening && data->data_len == 1024) {
                    i2s_tx.write_data((char*)data->data_ptr, data->data_len);
                } else {
                    unsigned long timeResponse = atoi((char*)data->data_ptr);
                    actionCommand(timeResponse);
                }
            }
            ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", 
                        data->payload_len, data->data_len, data->payload_offset);
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
            break;
    }
}

static void websocket_app_start(void) {
    esp_websocket_client_config_t websocket_cfg = {};

    websocket_cfg.host = websockets_server_host;
    websocket_cfg.port = websockets_server_port;

    ESP_LOGI(TAG, "Connecting to %s:%d...", websocket_cfg.host, websocket_cfg.port);

    socket = websocket_client(&websocket_cfg);
    socket.register_events(WEBSOCKET_EVENT_ANY, websocket_event_handler, NULL);

    ESP_ERROR_CHECK(socket.start());
}

void init_button(void) {
    xTaskCreate(button_task, "button_task", 4096, NULL, 0, NULL);
}