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
        EXCP_MODULE_NONVOLATILE_DATA = 100,
        EXCP_MODULE_DATASET = 110,
        EXCP_MODULE_EOL = 120,
        EXCP_MODULE_AUTOSAR_BASE = 500,
    } ExceptionModuleID;

    typedef enum
    {
        EXCP_TYPE_NULLPOINTER = 100,
        EXCP_TYPE_PARMETER,
        EXCP_TYPE_UNEXPECTED_VALUE,
        EXCP_TYPE_EXC_FAILED,
        EXCP_TYPE_EXCP_HANDLER_WRITING_DATA_FLASH_FAILED,
        EXCP_TYPE_EXCP_HANDLER_READING_DATA_FLASH_FAILED,
        EXCP_TYPE_PWM_IC_INIT_FAILED,
        EXCP_TYPE_PWM_IC_DEINIT_FAILED,
        EXCP_TYPE_SPEED_SENSOR_CONVERTER_PWM_READ_FAILED,
        EXCP_TYPE_SPEED_SENSOR_CONVERTER_VALID_SPEED_RANGE_EXCEEEDED,
        EXCP_TYPE_FUEL_SIGNAL_CONVERTER_RESTARTED_CONVERSION,
        EXCP_TYPE_NONVOLATILE_DATA_DUPLICATE_SECTION_NAME = 200,
        EXCP_TYPE_NONVOLATILE_DATA_SETTING_DEFAULT_SECTIONS_FAILED,
        EXCP_TYPE_NONVOLATILE_DATA_SECTION_TOO_LARGE,
        EXCP_TYPE_NONVOLATILE_DATA_SECTION_ADD_FAILED,
        EXCP_TYPE_NONVOLATILE_DATA_LOADING_FAILED,
        EXCP_TYPE_NONVOLATILE_DATA_RESIZE_FAILED,
        EXCP_TYPE_NONVOLATILE_DATA_INTEGRITY_BUFFER_TO_LARGE,
        EXCP_TYPE_NONVOLATILE_DATA_INTEGRITY_BUFFER_TOO_SMALL,
        EXCP_TYPE_NONVOLATILE_DATA_INTEGRITY_OVERLAPPING_SECTIONS,
        EXCP_TYPE_NONVOLATILE_DATA_WRITE_FAILED,
        EXCP_TYPE_NONVOLATILE_DATA_READ_FAILED,
        EXCP_TYPE_NONVOLATILE_DATA_ERASE_FAILED,
        EXCP_TYPE_DATASET_LOADING_FAILED = 300,
        EXCP_TYPE_DATASET_WRITING_FAILED,
        EXCP_TYPE_EOL_DATA_LOADING_FAILED = 320,
        EXCP_TYPE_EOL_DATA_WRITING_FAILED,
        EXCP_TYPE_AUTOSAR_BASE = 500
    } ExceptionTypeID;

    void ExceptionHandler_handle_exception(
            ExceptionModuleID en_module_id,
            ExceptionTypeID en_exception_id,
            bool bo_critical,
            const char* pci8_file,
            uint32_t u32_line, int32_t i32_misc);

#ifdef __cplusplus
} // Extern "C"
#endif


#endif
