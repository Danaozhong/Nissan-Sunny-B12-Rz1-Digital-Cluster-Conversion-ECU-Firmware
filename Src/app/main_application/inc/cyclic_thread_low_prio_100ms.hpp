#ifndef _CYCLIC_THREAD_LOW_PRIO_100MS_HPP_
#define _CYCLIC_THREAD_LOW_PRIO_100MS_HPP_

#include "thread_base.hpp"

namespace app
{
    class CyclicThreadLowPrio100ms : public os_services::CyclicThread
    {
    public:
        CyclicThreadLowPrio100ms(const char* name, size_t priority, size_t stack_size, std::chrono::milliseconds cycle_time_in_ms)
            : os_services::CyclicThread(name, priority, stack_size, cycle_time_in_ms)
        {}
        
        virtual ~CyclicThreadLowPrio100ms() {}
        
        virtual void process();
    };
}

#endif /* _CYCLIC_THREAD_100MS_HPP_ */