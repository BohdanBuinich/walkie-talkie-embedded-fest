set(headers
        timer.h
        )

idf_component_register(
        SRCS
        ${headers}
        INCLUDE_DIRS .
)