set(headers
        futurecpp.h
        websocket_client.h
        )

set(dependencies
        esp_websocket_client
        )

idf_component_register(
        SRCS
        ${headers}
        INCLUDE_DIRS .
        REQUIRES
        ${dependencies}
)