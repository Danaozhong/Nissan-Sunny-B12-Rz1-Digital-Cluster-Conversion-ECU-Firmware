
if (USE_CAN)

    SET(CAN_SOURCES
        Can/Can.c
        CanIf/CanIf.c
        CanIf/CanIf_Cfg.c
        CanIf/CanIf_Lcfg.c
        config/Can_Lcfg.c
        Det/Det.c
    )
    
    add_library(LIB_CAN STATIC ${CAN_SOURCES})
    
    target_include_directories(LIB_CAN PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMSIS_INCLUDE_DIRS}
        ${STM32HAL_INCLUDE_DIR}
    )

    target_include_directories(LIB_CAN PUBLIC
        ../../../Inc # TODO find a better solution for this include
        Can
        CanIf
        config
        Det
        EcuM
        PduR
        SchM
        shared
    )
    
    
endif()