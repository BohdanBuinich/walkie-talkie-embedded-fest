set(sources main.cpp)

set(dependencies
        protocol_examples_common
        nvs_flash
        I2S
        Button
        WebSocket
        Timer
        )

idf_component_register(
        SRCS
        ${sources}
        INCLUDE_DIRS .
        REQUIRES
        ${dependencies}
)