
set(MODULE_SOURCES
    src/console_commands.cpp
)
add_library(console_commands STATIC ${MODULE_SOURCES})

target_link_libraries(console_commands osservices)
target_link_libraries(console_commands main_application)
target_link_libraries(console_commands version_info)
target_link_libraries(console_commands ascii_graph)

target_include_directories(console_commands PUBLIC
    inc
)