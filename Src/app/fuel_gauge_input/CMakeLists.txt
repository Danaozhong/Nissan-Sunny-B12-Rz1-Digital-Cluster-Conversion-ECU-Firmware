
set(MODULE_SOURCES
    src/fuel_gauge_input.cpp
)

# Module options

# Enable this if you want to have additional log output for the fuel sensor acquisition module *
option(FUEL_GAUGE_INPUT_ENABLE_LOGGING "Enable / disable additional logging for the fuel input module" OFF)

# Enable if you want to run the fuel input module in an own task context
option(FUEL_GAUGE_INPUT_USE_OWN_TASK "Run fuel acquisition in an own task" OFF)

# Define the number of input readings to be maximum taken into account 
set(FUEL_GAUGE_INPUT_AVERAGING_SIZE "(7u)")


add_library(fuel_gauge_input STATIC ${MODULE_SOURCES})

target_link_libraries(fuel_gauge_input adc)
target_link_libraries(fuel_gauge_input excp_handler)
target_link_libraries(fuel_gauge_input event_handler)
target_link_libraries(fuel_gauge_input lookup_table)
target_link_libraries(fuel_gauge_input event_handler)

if(${FUEL_GAUGE_INPUT_ENABLE_LOGGING})
    target_compile_definitions(fuel_gauge_input PRIVATE
        FUEL_GAUGE_INPUT_ENABLE_LOGGING
    )
endif()

if(${FUEL_GAUGE_INPUT_USE_OWN_TASK})
    target_compile_definitions(fuel_gauge_input PRIVATE
        FUEL_GAUGE_INPUT_USE_OWN_TASK
    )
endif()

target_compile_definitions(fuel_gauge_input PUBLIC
    FUEL_GAUGE_INPUT_AVERAGING_SIZE=${FUEL_GAUGE_INPUT_AVERAGING_SIZE}
)

target_include_directories(fuel_gauge_input PUBLIC
    inc
)