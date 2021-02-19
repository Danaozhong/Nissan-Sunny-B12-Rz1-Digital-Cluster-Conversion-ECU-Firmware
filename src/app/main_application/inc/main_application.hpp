#ifndef _MAIN_APPLICATION_HPP_
#define _MAIN_APPLICATION_HPP_

/* Own headers */
#include "fuel_gauge_input.hpp"
#include "fuel_gauge_output.hpp"
#include "lookup_table.hpp"
#include "lookup_table_editor.hpp"

#include "speed_sensor_converter.hpp"

#include "os_console.hpp"

#include "cyclic_thread_100ms.hpp"
#include "cyclic_thread_low_prio_100ms.hpp"

#include "excp_handler.hpp"

#include "stm32_dac.hpp"
#include "stm32_adc.hpp"
#include "stm32_uart.hpp"
#include "stm32_pwm.hpp"
#include "stm32_pwm_ic.hpp"

#ifdef USE_NVDH
#include "nonvolatile_data_handler.hpp"
#endif /* USE_NVDH */
#include "os_console.hpp"
#include "ex_thread.hpp"
#include "dataset.hpp"
#ifdef USE_TRACE
#include "trace.hpp"
#endif
#include "eol.hpp"

namespace app
{
    extern drivers::STM32PWM* po_dummy_pwm;

    enum FuelGaugeOutputMode
    {
        FUEL_GAUGE_OUTPUT_MODE_CONVERSION,
        FUEL_GAUGE_OUTPUT_MODE_MANUAL
    };

    class MainApplication
    {
    public:
        /** Constructor */
        MainApplication();

        auto startup_from_reset() -> void;

        auto startup_from_wakeup() -> void;

        auto go_to_sleep() -> void;

        auto init_hardware() -> void;

        auto deinit_hardware() -> void;

        /** Callback used when the fuel sensor input has detected an input level change */
        auto fuel_sensor_input_received(int32_t i32_value) -> void;

        auto update_fuel_sensor_output() -> void;

        /** Singleton accessor */
        static auto get() -> MainApplication&;

        auto get_speed_sensor_converter() const -> SpeedSensorConverter*;

        /** Cyclic container to be executed every 100ms, under a low task priority */
        auto cycle_low_prio_100ms() -> void;

        /** Cyclic container to be executed every 100ms */
        auto cycle_100ms() -> void;

        /** Cyclic container to be executed every 1 second */
        auto cycle_1000ms() -> void;

        /** this thread will take care of user I/O interface, and user commands */
        auto console_thread() -> void;

#ifdef USE_NVDH
        /** \brief Returns the data handler for accessing nonvolatile data */
        auto get_nonvolatile_data_handler() const -> midware::NonvolatileDataHandler*;
#endif
        /** \brief Retuns a pointer to the console interface */
        auto get_os_console() -> OSServices::OSConsole*;

        /** \brief Returns the standard I/O interface configured for the OS console object */
        auto get_stdio() -> OSServices::OSConsoleGenericIOInterface&;

        /** Use this to select the mode in which the speed signal is sent out to the cluster.
         * use OUTPUT_MODE_CONVERSION to derive the speed signal from the input speed sensor,
         * alternatively; use OUTPUT_MODE_MANUAL to manually configure a speed value. */
        auto set_fuel_gauge_output_mode(FuelGaugeOutputMode en_speed_output_mode) -> void;

        /** \brief When the speed sensor conversion is in manual mode, use this function to set the manual
         * speed value.
         * \param[in] i32_speed_in_mph  The velocity in meters / hour.
         */
        auto set_manual_fuel_gauge_output_value(int32_t _i32_fuel_gauge_output_value) -> void;
        
        /** \brief returns the currently configured manual speeed in meters / hour. */
        auto get_manual_fuel_gauge_output_value() const -> int32_t;
        
        /** \brief Returns the dataset (const). */
        auto get_dataset() const -> const app::Dataset&;
        
        /** \brief Returns the dataset (nonconst). */
        auto get_dataset() -> app::Dataset&;
        
        auto get_fuel_gauge_input() const -> const app::FuelGaugeInputFromADC*;
        
        auto get_fuel_gauge_output() const -> const app::FuelGaugeOutput*;

        auto get_eol_data() -> app::EOLData&;

    private:
        // prevent copying
        MainApplication(MainApplication &other) = delete;

        auto init_speed_converter() -> int32_t;

        auto deinit_speed_converter() -> int32_t;

        auto init_fuel_level_converter() -> int32_t;
        auto deinit_fuel_level_converter() -> int32_t;

        /** \brief the UART object used */
        drivers::GenericUART* m_p_uart;
        OSServices::OSConsole* m_po_os_console;
        OSServices::OSConsoleGenericIOInterface* m_po_os_io_interface;

        midware::ExceptionHandler* m_po_exception_handler;
#ifdef USE_TRACE
        midware::Trace* m_po_trace;
#endif

        drivers::GenericADC* m_p_adc;
        drivers::GenericDAC* m_p_dac;

        /// the PWM output used to simulate the JDM speed sensor to the cluster
        drivers::GenericPWM* m_p_pwm;

        /// The PWM Input Capture driver to read the PWM from the vehicle's speed sensor
        drivers::GenericPWM_IC* m_p_pwm_ic;

        app::FuelGaugeInputFromADC* m_p_o_fuel_gauge_input;
        app::FuelGaugeOutput* m_p_o_fuel_gauge_output;

        FuelGaugeOutputMode m_en_fuel_gauge_output_mode;
        SpeedSensorConverter* m_po_speed_sensor_converter;

        CyclicThreadLowPrio100ms m_o_cyclic_thread_low_prio_100ms;
        CyclicThread100ms m_o_cyclic_thread_100ms;
        
        app::Dataset m_o_dataset;

        app::EOLData m_o_eol_data;

        int32_t m_i32_fuel_sensor_read_value;
        int32_t m_i32_fuel_gauge_output_manual_value;
                
#ifdef USE_NVDH
        midware::NonvolatileDataHandler* m_po_nonvolatile_data_handler;
#endif /* USE_NVDH */
    };
}
#endif /* _MAIN_APPLICATION_HPP_ */
