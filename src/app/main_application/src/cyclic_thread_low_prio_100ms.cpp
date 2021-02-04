#include "cyclic_thread_low_prio_100ms.hpp"
#include "main_application.hpp"

namespace app
{
    void CyclicThreadLowPrio100ms::process()
    {
        app::MainApplication& o_application = app::MainApplication::get();
        
        o_application.cycle_low_prio_100ms();
    }
}




