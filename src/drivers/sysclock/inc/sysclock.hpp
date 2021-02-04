#ifndef _SYSCLOCK_HPP_
#define _SYSCLOCK_HPP_

#include <cstdint>

namespace drivers
{
    class SysClock
    {
    public:
        SysClock();
        
        void configure();
        
        uint32_t u32_get_sysclock() const;
    private:
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