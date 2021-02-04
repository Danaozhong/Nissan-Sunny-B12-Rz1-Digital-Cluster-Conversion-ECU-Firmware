#include "cyclic_thread_low_prio_100ms.hpp"
#include "main_application.cpp"

namespace app
{
    CyclicThreadLowPrio100ms::process()
    {
        app::MainApplication& o_application = app::MainApplication::get();
        
        o_application.cycle_low_prio_100ms();
    }
}




