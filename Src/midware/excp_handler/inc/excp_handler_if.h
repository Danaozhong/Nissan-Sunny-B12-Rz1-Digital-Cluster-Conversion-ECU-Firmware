#ifndef _ERROR_STORAGE_IF_H_
#define _ERROR_STORAGE_IF_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        EXCP_MODULE_ADC,
        EXCP_MODULE_DAC,
        EXCP_MODULE_PWM_IC,
        EXCP_MODULE_SPEED_SENSOR_CONVERTER,
        EXCP_MODULE_FUEL_SIGNAL_CONVERTER,
        EXCP_MODULE_EXCP_HANDLER,
        EXCP_MODULE_NONVOLATILE_DATA,
        EXCP_MODULE_AUTOSAR_BASE = 500,
    } ExceptionModuleID;

    typedef enum
    {
        EXCP_TYPE_NULLPOINTER = 100,
        EXCP_TYPE_PARMETER,
        EXCP_TYPE_EXC_FAILED,
        EXCP_TYPE_EXCP_HANDLER_WRITING_DATA_FLASH_FAILED,
        EXCP_TYPE_EXCP_HANDLER_READING_DATA_FLASH_FAILED,
        EXCP_TYPE_PWM_IC_INIT_FAILED,
        EXCP_TYPE_PWM_IC_DEINIT_FAILED,
        EXCP_TYPE_SPEED_SENSOR_CONVERTER_PWM_READ_FAILED,
        EXCP_TYPE_SPEED_SENSOR_CONVERTER_VALID_SPEED_RANGE_EXCEEEDED,
        EXCP_TYPE_NONVOLATILE_DATA_LOADING_FAILED,
        EXCP_TYPE_AUTOSAR_BASE = 500
    } ExceptionTypeID;

    void ExceptionHandler_handle_exception(
            ExceptionModuleID en_module_id,
            ExceptionTypeID en_exception_id,
            bool bo_critical,
            const char* pci8_file,
            uint32_t u32_line, uint32_t u32_misc);

#ifdef __cplusplus
} // Extern "C"
#endif


#endif
