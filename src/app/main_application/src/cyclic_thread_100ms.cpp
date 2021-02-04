#include "cyclic_thread_100ms.hpp"
#include "main_application.hpp"

namespace app
{
    void CyclicThread100ms::process()
    {
        app::MainApplication& o_application = app::MainApplication::get();
        static int32_t counter = 0;
        
        o_application.cycle_100ms();
        counter++;
        if (counter > 10)
        {
            counter = 0;
            o_application.cycle_1000ms();
        }
    }
}
