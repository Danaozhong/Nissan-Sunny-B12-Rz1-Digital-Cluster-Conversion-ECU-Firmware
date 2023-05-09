
SET(MODULE_SOURCES
        src/thread_base.cpp
)


add_library(os_threads STATIC ${MODULE_SOURCES})

target_link_libraries(os_threads PUBLIC 
    freertos
    trace
)


target_include_directories(os_threads PUBLIC
    inc

)
