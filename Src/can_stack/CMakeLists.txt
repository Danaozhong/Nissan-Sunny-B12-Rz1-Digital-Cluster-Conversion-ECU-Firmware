
SET(CAN_SOURCES
    Can/Can.c
    CanIf/CanIf.c
    CanIf/CanIf_Cfg.c
    CanIf/CanIf_Lcfg.c
    config/Can_Lcfg.c
    Det/Det.c
)


add_library(can_stack STATIC ${CAN_SOURCES})

target_compile_definitions(can_stack PUBLIC
    -DHAL_CAN_MODULE_ENABLED
)

target_include_directories(can_stack PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
)

target_include_directories(can_stack PUBLIC
    ../../Inc # TODO find a better solution for this include
    Can
    CanIf
    config
    Det
    EcuM
    PduR
    SchM
    shared
)