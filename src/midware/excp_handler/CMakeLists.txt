
SET(MODULE_SOURCES
    src/excp_handler.cpp
    src/excp_handler_console_commands.cpp
)

add_library(excp_handler STATIC ${MODULE_SOURCES})

if (USE_NVDH)
    target_link_libraries(excp_handler PUBLIC nonvolatile_data_handler)
endif()
target_link_libraries(excp_handler PUBLIC
    util_algorithms
    libtable
    trace
    watchdog  # To be able to trigger a reset, if needed
)

target_include_directories(excp_handler PUBLIC
    inc

)
