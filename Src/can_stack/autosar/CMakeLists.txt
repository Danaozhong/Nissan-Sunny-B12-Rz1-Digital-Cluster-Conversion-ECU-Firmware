
if (${USE_CAN} EQUAL ON)
    add_definitions(
        -DUSE_CAN
    )
    
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/Can
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/CanIf
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/config
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/Det
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/EcuM
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/PduR
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/SchM
        ${CMAKE_CURRENT_SOURCE_DIR}/can_stack/autosar/shared
    )

    list(append PROJECT_SOURCES
        can_stack/autosar/Can/Can.c
        can_stack/autosar/CanIf/CanIf.c
        can_stack/autosar/CanIf/CanIf_Cfg.c
        can_stack/autosar/CanIf/CanIf_Lcfg.c
        can_stack/autosar/config/Can_Lcfg.c
        can_stack/autosar/Det/Det.c
    )
endif()