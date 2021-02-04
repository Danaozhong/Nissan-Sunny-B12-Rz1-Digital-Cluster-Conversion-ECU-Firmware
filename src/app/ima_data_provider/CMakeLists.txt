
SET(MODULE_SOURCES
    src/ima_data_provider.cpp
)


add_library(ima_data_provider STATIC ${MODULE_SOURCES})

target_link_libraries(ima_data_provider can_stack)
target_link_libraries(ima_data_provider trace)

#target_include_directories(can_stack PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMSIS_INCLUDE_DIRS}
#    ${STM32HAL_INCLUDE_DIR}
#    ../../Inc # needed for HW depending stuff, TODO find a better solution for this include
#)

target_include_directories(ima_data_provider PUBLIC
    inc
)