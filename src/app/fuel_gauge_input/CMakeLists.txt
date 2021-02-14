
set(MODULE_SOURCES
    src/fuel_gauge_input.cpp
)

# Module options

# Define the number of raw ADC input readings to be taken to calculate one average fuel input value
set(FUEL_GAUGE_INPUT_AVERAGING_SIZE 10)

# Defines the number of averaged fuel values to be taken into account for the algorithm
set(FUEL_GAUGE_INPUT_NUM_OF_AVERAGES 10)

# Set the max spread of FUEL_GAUGE_INPUT_AVERAGING_SIZE raw sensor readings under which 
# consider these values acceptable. Unit is % * 100.
set(FUEL_GAUGE_INPUT_READINGS_MAX_DISTANCE 300)

# If an average reading has a min/max spread larger than this threshold, the value will be ignored entirely.
# Unit is % * 100.
set(FUEL_GAUGE_INPUT_READINGS_INVALID_DATA_DISTANCE 700)

# Specifies the thresold to the currently set fuel values up and down. If these thresholds are exceeded for
# more than FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD, a new recalibration will be triggered.
# This is the upper threshold (read fuel value is more than displayed). Unit is % * 100. If exceeded, will trigger refuel detection.
set(FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_UP 1000)

# This is the bottom threshold (read fuel value is less than displayed). Unit is % * 100. If exceeded, will trigger error recalibration.
set(FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_DOWN 1000)

# In case FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_UP/FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_DOWN are triggered for consecutive 
# FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD times, the recalibration/refill detection will be triggered.
# Unit is cycles (usually 1 second)
set(FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD 30)

# If due to the fuel usage the read sensor is slighly less than what is displayed, the display value must be updated.
# The update will only happen if FUEL_GAUGE_INPUT_FUEL_LOWER_COUNTER_THRESHOLD averaged readings differ by more than
# FUEL_GAUGE_INPUT_PERCENTAGE_UPDATE_THRESHOLD from the current displayed value.
# Unit is in % * 100.
set(FUEL_GAUGE_INPUT_PERCENTAGE_UPDATE_THRESHOLD 50)

# This specifies how many cycles the read fuel value must be less than the displayed value, until the displayed value gets updated.
# Unit is cycles (usually 1 second)
set(FUEL_GAUGE_INPUT_FUEL_LOWER_COUNTER_THRESHOLD 5)

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

target_compile_definitions(fuel_gauge_input PRIVATE
    FUEL_GAUGE_INPUT_AVERAGING_SIZE=${FUEL_GAUGE_INPUT_AVERAGING_SIZE}
    FUEL_GAUGE_INPUT_NUM_OF_AVERAGES=${FUEL_GAUGE_INPUT_NUM_OF_AVERAGES}
    FUEL_GAUGE_INPUT_READINGS_MAX_DISTANCE=${FUEL_GAUGE_INPUT_READINGS_MAX_DISTANCE}
    FUEL_GAUGE_INPUT_READINGS_INVALID_DATA_DISTANCE=${FUEL_GAUGE_INPUT_READINGS_INVALID_DATA_DISTANCE}
    FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_UP=${FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_UP}
    FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_DOWN=${FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_DOWN}
    FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD=${FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD}
    FUEL_GAUGE_INPUT_PERCENTAGE_UPDATE_THRESHOLD=${FUEL_GAUGE_INPUT_PERCENTAGE_UPDATE_THRESHOLD}
    FUEL_GAUGE_INPUT_FUEL_LOWER_COUNTER_THRESHOLD=${FUEL_GAUGE_INPUT_FUEL_LOWER_COUNTER_THRESHOLD}
)

target_include_directories(fuel_gauge_input PUBLIC
    inc
)