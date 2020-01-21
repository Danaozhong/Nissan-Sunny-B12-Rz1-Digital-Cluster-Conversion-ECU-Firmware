SET(CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wall -std=gnu11 --specs=nano.specs -ffunction-sections -fomit-frame-pointer -fstack-usage" CACHE INTERNAL "c compiler flags")

SET(CMAKE_CXX_FLAGS "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wall -std=c++14 -ffunction-sections -fno-exceptions -fno-threadsafe-statics -fno-rtti -fno-use-cxa-atexit -fstack-usage --specs=nano.specs -mthumb" CACHE INTERNAL "cxx compiler flags")

SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -Wa,--no-warn -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard --specs=nosys.specs" CACHE INTERNAL "executable linker flags")
 
SET(CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16" CACHE INTERNAL "module linker flags")
SET(CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16" CACHE INTERNAL "shared linker flags")
SET(STM32_CHIP_TYPES 301xx 302xx 303xx 334xx 373xx CACHE INTERNAL "stm32f3 chip types")
SET(STM32_CODES "301.." "302.." "303.." "334.." "373..")

MACRO(STM32_GET_CHIP_TYPE CHIP CHIP_TYPE)
    STRING(REGEX REPLACE "^[sS][tT][mM]32[fF](3[037][1234].[68BC]).*$" "\\1" STM32_CODE ${CHIP})
    SET(INDEX 0)
    FOREACH(C_TYPE ${STM32_CHIP_TYPES})
        LIST(GET STM32_CODES ${INDEX} CHIP_TYPE_REGEXP)
        IF(STM32_CODE MATCHES ${CHIP_TYPE_REGEXP})
            SET(RESULT_TYPE ${C_TYPE})
        ENDIF()
        MATH(EXPR INDEX "${INDEX}+1")
    ENDFOREACH()
    SET(${CHIP_TYPE} ${RESULT_TYPE})
ENDMACRO()

MACRO(STM32_GET_CHIP_PARAMETERS CHIP FLASH_SIZE RAM_SIZE CCRAM_SIZE)
    STRING(REGEX REPLACE "^[sS][tT][mM]32[fF](3[037][1234].[68BC]).*$" "\\1" STM32_CODE ${CHIP})
    STRING(REGEX REPLACE "^[sS][tT][mM]32[fF]3[037][1234].([68BC]).*$" "\\1" STM32_SIZE_CODE ${CHIP})

    IF(STM32_SIZE_CODE STREQUAL "6")
        SET(FLASH "32K")
        SET(CCRAM_SIZE_IN_K "4")
    ELSEIF(STM32_SIZE_CODE STREQUAL "8")
        SET(FLASH "64K")
        SET(CCRAM_SIZE_IN_K "4")
    ELSEIF(STM32_SIZE_CODE STREQUAL "B")
        SET(FLASH "128K")
        SET(CCRAM_SIZE_IN_K "8")
    ELSEIF(STM32_SIZE_CODE STREQUAL "C")
        SET(FLASH "256K")
        SET(CCRAM_SIZE_IN_K "8")
    ENDIF()

    STM32_GET_CHIP_TYPE(${CHIP} TYPE)

    IF(${TYPE} STREQUAL "301xx")
        SET(RAM_SIZE_IN_K "16")
    ELSEIF(${TYPE} STREQUAL "302xx")
        SET(RAM_SIZE_IN_K "256")
    ELSEIF(${TYPE} STREQUAL "303xx")
        SET(RAM_SIZE_IN_K "48")
    ELSEIF(${TYPE} STREQUAL "334xx")
        SET(RAM_SIZE_IN_K "16")
    ELSEIF(${TYPE} STREQUAL "373xx")
        SET(RAM_SIZE_IN_K "128")
    ENDIF()

    # RAM size = total RAM - CCRAM
    MATH(EXPR RAM_SIZE_IN_K "${RAM_SIZE_IN_K}-${CCRAM_SIZE_IN_K}")
    SET(RAM "${RAM_SIZE_IN_K}K")
    SET(CCRAM "${CCRAM_SIZE_IN_K}K")
    
    
    SET(${FLASH_SIZE} ${FLASH})
    SET(${RAM_SIZE} ${RAM})
    SET(${CCRAM_SIZE} ${CCRAM})
ENDMACRO()

FUNCTION(STM32_SET_CHIP_DEFINITIONS TARGET CHIP_TYPE)
    LIST(FIND STM32_CHIP_TYPES ${CHIP_TYPE} TYPE_INDEX)
    IF(TYPE_INDEX EQUAL -1)
        MESSAGE(FATAL_ERROR "Invalid/unsupported STM32F3 chip type: ${CHIP_TYPE}")
    ENDIF()
    GET_TARGET_PROPERTY(TARGET_DEFS ${TARGET} COMPILE_DEFINITIONS)
    STRING(REGEX REPLACE "^(3..).(.)" "\\1x\\2" CHIP_TYPE_2 ${STM32_CODE})
    IF(TARGET_DEFS)
        SET(TARGET_DEFS "STM32F3;STM32F${CHIP_TYPE_2};${TARGET_DEFS}")
    ELSE()
        SET(TARGET_DEFS "STM32F3;STM32F${CHIP_TYPE_2}")
    ENDIF()

    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES COMPILE_DEFINITIONS "${TARGET_DEFS}")
ENDFUNCTION()
