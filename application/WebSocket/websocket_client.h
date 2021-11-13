#pragma once

// system includes
#include <utility>
#include <string_view>
#include <string>

// espressif includes
#include <esp_websocket_client.h>
#include <esp_transport_ws.h>

// local includes
#include "futurecpp.h"

class websocket_client {
public:
    websocket_client() : handle{NULL} {}

    websocket_client(const esp_websocket_client_config_t *config) : handle{esp_websocket_client_init(config)} {}

    websocket_client(const websocket_client &) = delete;

    websocket_client(websocket_client &&other) : handle{std::move(other.handle)} { other.handle = NULL; }

    websocket_client &operator=(const websocket_client &) = delete;

    websocket_client &operator=(websocket_client &&other) {
        if (handle) esp_websocket_client_destroy(handle);
        handle = std::move(other.handle);
        other.handle = NULL;
        return *this;
    }

    ~websocket_client() { if (handle) esp_websocket_client_destroy(handle); }

    operator bool() const { return handle != NULL; }

    //esp_err_t set_uri        (const char *uri)                                         { return esp_websocket_client_set_uri        (handle, uri);                      }
    esp_err_t set_uri(std::string_view uri) { return esp_websocket_client_set_uri(handle, uri.data()); }

    esp_err_t start() { return esp_websocket_client_start(handle); }

    esp_err_t stop() { return esp_websocket_client_stop(handle); }

    //int       send           (const char *data, int len, TickType_t timeout)           { return esp_websocket_client_send           (handle, data, len, timeout);       }
    int send(std::string_view buf, TickType_t timeout) {
        return esp_websocket_client_send(handle, buf.data(), buf.size(), timeout);
    }

    int send_bin(const char *data, int len, TickType_t timeout) {
        return esp_websocket_client_send_bin(handle, data, len, timeout);
    }

    // int       send_bin       (std::string_view buf, TickType_t timeout)                { return esp_websocket_client_send_bin       (handle, buf.data(), buf.size(), timeout); }
    int send_text(const char *data, int len, TickType_t timeout) {
        return esp_websocket_client_send_text(handle, data, len, timeout);
    }

    // int       send_text      (std::string_view buf, TickType_t timeout)                { return esp_websocket_client_send_text      (handle, buf.data(), buf.size(), timeout); }
    esp_err_t close(TickType_t timeout) { return esp_websocket_client_close(handle, timeout); }

    //esp_err_t close_with_code(int code, const char *data, int len, TickType_t timeout) { return esp_websocket_client_close_with_code(handle, code, data, len, timeout); }
    esp_err_t close_with_code(int code, std::string_view buf, TickType_t timeout) {
        return esp_websocket_client_close_with_code(handle, code, buf.data(), buf.size(), timeout);
    }

    bool is_connected() const { return esp_websocket_client_is_connected(handle); }

    esp_err_t register_events(esp_websocket_event_id_t event, esp_event_handler_t event_handler,
                              void *event_handler_arg) {
        return esp_websocket_register_events(handle, event, event_handler, event_handler_arg);
    }

    static std::string opcodeToString(ws_transport_opcodes op_code) {
        switch (op_code) {
            case WS_TRANSPORT_OPCODES_CONT:
                return "CONT";
            case WS_TRANSPORT_OPCODES_TEXT:
                return "TEXT";
            case WS_TRANSPORT_OPCODES_BINARY:
                return "BINARY";
            case WS_TRANSPORT_OPCODES_CLOSE:
                return "CLOSE";
            case WS_TRANSPORT_OPCODES_PING:
                return "PING";
            case WS_TRANSPORT_OPCODES_PONG:
                return "PONG";
            case WS_TRANSPORT_OPCODES_FIN:
                return "FIN";
            default:
                return "unknown opcode(" + std::to_underlying(op_code) + ')';
        }
    }

private:
    esp_websocket_client_handle_t handle;
};
