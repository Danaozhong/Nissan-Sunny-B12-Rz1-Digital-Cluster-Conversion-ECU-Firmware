#ifdef USE_CAN
#include "CanIf.h"
#endif

#include "trace_if.h"


/* OS headers */
#include "mcu_interface.hpp"
#include "uc_ports.hpp"
#include "main_application.hpp"
#include "watchdog.hpp"


auto MAIN_startup_thread(void*) -> void
{
    // create our main application.
    app::MainApplication& o_application = app::MainApplication::get();
    o_application.startup_from_reset();
    o_application.console_thread();
    vTaskDelete(NULL);
}

auto main(void) -> int
{
    // configure the system clock
    drivers::McuInterface::get_instance().configure_system_clock();
    
    HAL_Init();
    
    // create the watchdog
    midware::Watchdog::get_instance().activate(2000); // set the watchdog with a timeout of 1 second.
    
    // create the port configuration object, and configure the processor ports
    drivers::STM32F303CCT6UcPorts* po_uc_port_configuration = new drivers::STM32F303CCT6UcPorts();
    po_uc_port_configuration->configure();
    
    // first thread still needs to be created with xTaskCreate, only after the scheduler has started, std::thread can be used.
    TaskHandle_t xHandle = NULL;
    xTaskCreate( MAIN_startup_thread,
                 "MAIN_startup_thread",
                 0x600,
                 NULL,
                 3u,
                 &xHandle );

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */

    for( ;; );
}
