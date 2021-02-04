
SET(MODULE_SOURCES
    src/eeprom.c
)

add_library(eeprom STATIC ${MODULE_SOURCES})

target_include_directories(eeprom PUBLIC
    ../../../Inc # needed for HW depending stuff, TODO find a better solution for this include
)

target_include_directories(eeprom PUBLIC
    inc
)

target_link_libraries(eeprom PRIVATE 
   HAL::STM32::F3::FLASH
)