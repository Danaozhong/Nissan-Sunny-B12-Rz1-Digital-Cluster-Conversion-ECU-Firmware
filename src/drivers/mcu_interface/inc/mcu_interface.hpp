#ifndef _SYSCLOCK_HPP_
#define _SYSCLOCK_HPP_

#include <cstdint>

namespace drivers
{
    /** Enumeration with the reset reasons */
    enum SystemResetReason
    {
        RESET_REASON_WATCHDOG_RESET,
        RESET_REASON_SOFTWARE_RESET,
        RESET_REASON_POWER_FAIL,
        RESET_REASON_POWER_ON_RESET,
        RESET_REASON_UNKNOWN
    };
    
    class McuInterface
    {
    public:
        static McuInterface& get_instance()
        {
            static McuInterface o_instance;
            return o_instance;
        }
        
        /** Configures the system clock */
        void configure_system_clock();
        
        /** returns the system clock in Hz */
        uint32_t u32_get_sysclock() const;
        
        /** returns the current reset reason */
        SystemResetReason get_reset_reason() const;
        
        McuInterface(const McuInterface&) = delete;
        void operator=(const McuInterface&) = delete;
    private:
        McuInterface() {}
#ifdef USE_STM32F3_DISCO
        void SystemClock_Config_STM32_F3_DISCOVERY();
#elif defined USE_STM32F3XX_NUCLEO_32
        void SystemClock_Config_STM32F303_NUCLEO_32();
#elif defined STM32F303xC
        void SystemClock_Config_STM32F303xC();
#elif define STM32F429xx
        void SystemClock_Config_STM32F429xx();
#endif
    };
}
#endif
