
SET(MODULE_SOURCES
    src/excp_handler.cpp
    src/excp_handler_console_commands.cpp
)

add_library(excp_handler STATIC ${MODULE_SOURCES})

target_link_libraries(excp_handler trace)
if (USE_NVDH)
    target_link_libraries(excp_handler nonvolatile_data_handler)
endif()
target_link_libraries(excp_handler util_algorithms)
target_link_libraries(excp_handler libtable)

target_include_directories(excp_handler PUBLIC
    inc

)
