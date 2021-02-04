
SET(MODULE_SOURCES
    src/nonvolatile_data_handler.cpp
)


add_library(nonvolatile_data_handler STATIC ${MODULE_SOURCES})

target_link_libraries(nonvolatile_data_handler trace)
target_link_libraries(nonvolatile_data_handler eeprom)
target_link_libraries(nonvolatile_data_handler excp_handler)

target_compile_definitions(nonvolatile_data_handler PUBLIC
    USE_NVDH
)

target_include_directories(nonvolatile_data_handler PUBLIC
    inc
)