set(sources
        button.c
        )

set(dependencies
        log
        driver
        )

idf_component_register(
        SRCS
        ${sources}
        INCLUDE_DIRS .
        REQUIRES
        ${dependencies}
)