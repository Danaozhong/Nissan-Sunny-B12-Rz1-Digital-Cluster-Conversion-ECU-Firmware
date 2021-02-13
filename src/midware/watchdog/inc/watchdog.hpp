#ifndef _WATCHDOG_HPP_
#define _WATCHDOG_HPP_

#include <cstdint>
#include <atomic>

#include "stm32fxxx.h"

namespace midware
{
    class Watchdog
    {
    public:
        static Watchdog& get_instance()
        {
            static Watchdog o_watchdog;
            return o_watchdog;
        }
        void activate(uint32_t num_of_ms);
        void trigger();
        
        /// call this if you want to reset the system
        void trigger_reset();
        
        Watchdog() : m_bo_trigger_reset(false) {}
        Watchdog(Watchdog const&) = delete;
        void operator=(Watchdog const&) = delete;
    private:
        IWDG_HandleTypeDef m_o_handle;
        
        std::atomic<bool> m_bo_trigger_reset;
    };
    
    
}

#endif /* _WATCHDOG_HPP_ */
