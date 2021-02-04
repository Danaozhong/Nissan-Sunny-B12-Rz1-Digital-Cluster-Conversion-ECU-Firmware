
SET(MODULE_SOURCES
    src/eol.cpp
)


add_library(eol STATIC ${MODULE_SOURCES})

target_link_libraries(eol trace)
target_link_libraries(eol excp_handler)
target_link_libraries(eol nonvolatile_data_handler)

target_include_directories(eol PUBLIC
    inc
)