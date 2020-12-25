
SET(MODULE_SOURCES
    src/eol_command.cpp
)


add_library(eol_command STATIC ${MODULE_SOURCES})

target_link_libraries(eol_command eol)
target_link_libraries(eol_command osservices)

target_include_directories(eol_command PUBLIC
    inc
)