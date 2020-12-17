
SET(MODULE_SOURCES
    src/eeprom.c
)

add_library(eeprom STATIC ${MODULE_SOURCES})

target_include_directories(eeprom PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
    ../../../Inc # needed for HW depending stuff, TODO find a better solution for this include
)

target_include_directories(eeprom PUBLIC
    inc
)