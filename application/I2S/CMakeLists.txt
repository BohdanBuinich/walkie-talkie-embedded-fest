set(headers
        i2s.h
        )

set(dependencies
        driver
        )

idf_component_register(
        SRCS
        ${headers}
        INCLUDE_DIRS .
        REQUIRES
        ${dependencies}
)