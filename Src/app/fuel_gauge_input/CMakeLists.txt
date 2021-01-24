
set(MODULE_SOURCES
    src/fuel_gauge_input.cpp
)

# Module options

# Enable this if you want to have additional log output for the fuel sensor acquisition module *
option(FUEL_GAUGE_INPUT_ENABLE_LOGGING "Enable / disable additional logging for the fuel input module" OFF)

# Enable if you want to run the fuel input module in an own task context
option(FUEL_GAUGE_INPUT_USE_OWN_TASK "Run fuel acquisition in an own task" OFF)

# Define the number of raw ADC input readings to be taken to calculate one average fuel input value
set(FUEL_GAUGE_INPUT_AVERAGING_SIZE "(10u)")

# Defines the number of averaged fuel values to be taken into account for the algorithm
set(FUEL_GAUGE_INPUT_NUM_OF_AVERAGES 10)


add_library(fuel_gauge_input STATIC ${MODULE_SOURCES})

target_link_libraries(fuel_gauge_input adc)
target_link_libraries(fuel_gauge_input excp_handler)
target_link_libraries(fuel_gauge_input event_handler)
target_link_libraries(fuel_gauge_input lookup_table)
target_link_libraries(fuel_gauge_input event_handler)
target_link_libraries(fuel_gauge_input util_algorithms)

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
    FUEL_GAUGE_INPUT_NUM_OF_AVERAGES=${FUEL_GAUGE_INPUT_NUM_OF_AVERAGES}
)

target_include_directories(fuel_gauge_input PUBLIC
    inc
)