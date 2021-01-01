
set(MODULE_SOURCES
    src/fuel_gauge_output.cpp
)


add_library(fuel_gauge_output STATIC ${MODULE_SOURCES})

target_link_libraries(fuel_gauge_output dac)
target_link_libraries(fuel_gauge_output lookup_table)

target_include_directories(fuel_gauge_output PUBLIC
    inc
)