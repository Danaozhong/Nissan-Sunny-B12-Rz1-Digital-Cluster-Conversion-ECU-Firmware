
set(MODULE_SOURCES
    src/event_handler.cpp
)

add_library(event_handler STATIC ${MODULE_SOURCES})


target_include_directories(event_handler PUBLIC
    inc
)