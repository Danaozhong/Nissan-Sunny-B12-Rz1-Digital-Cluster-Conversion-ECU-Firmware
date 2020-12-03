#ifndef _MAIN_APPLICATION_HPP_
#define _MAIN_APPLICATION_HPP_

/* Own headers */
#include "fuel_gauge_input.hpp"
#include "fuel_gauge_output.hpp"
#include "lookup_table.hpp"
#include "lookup_table_editor.hpp"

#include "speed_sensor_converter.hpp"

#include "os_console.hpp"
#include "excp_handler.hpp"
#include "main.h"


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

		void startup_from_reset();

		void startup_from_wakeup();

		void go_to_sleep();

		void init_hardware();

		void deinit_hardware();

		/** Callback used when the fuel sensor input has detected an input level change */
		void fuel_sensor_input_received(int32_t i32_value);

		void update_fuel_sensor_output();

		/** Singleton accessor */
		static MainApplication& get();

		SpeedSensorConverter* get_speed_sensor_converter() const;

		/** Cyclic container to be executed every 10ms */
		void cycle_10ms();

		/** Cyclic container to be executed every 100ms */
		void cycle_100ms();

		/** Cyclic container to be executed every 1 second */
		void cycle_1000ms();

#ifdef USE_NVDH
        std::shared_ptr<midware::NonvolatileDataHandler> get_nonvolatile_data_handler() const;
#endif
		std::shared_ptr<OSServices::OSConsole> get_os_console();


        /** Use this to select the mode in which the speed signal is sent out to the cluster.
         * use OUTPUT_MODE_CONVERSION to derive the speed signal from the input speed sensor,
         * alternatively; use OUTPUT_MODE_MANUAL to manually configure a speed value. */
        void set_fuel_gauge_output_mode(FuelGaugeOutputMode en_speed_output_mode);

        /** When the speed sensor conversion is in manual mode, use this function to set the manual
         * speed value.
         * \param[in] i32_speed_in_mph  The velocity in meters / hour.
         */
        void set_manual_fuel_gauge_output_value(int32_t _i32_fuel_gauge_output_value);

        const app::Dataset& get_dataset() const;
        app::Dataset& get_dataset();
    //private:
        // prevent copying
        MainApplication(MainApplication &other) = delete;

        int32_t init_speed_converter();

        int32_t deinit_speed_converter();

        int32_t init_fuel_level_converter();
        int32_t deinit_fuel_level_converter();

        drivers::GenericUART* m_p_uart;
        std::shared_ptr<OSServices::OSConsole> m_po_os_console;
        std::shared_ptr<OSServices::OSConsoleGenericIOInterface> m_po_os_io_interface;

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

		app::Dataset m_o_dataset;

	    int32_t m_i32_fuel_sensor_read_value;
	    int32_t m_i32_fuel_gauge_output_manual_value;
#ifdef USE_NVDH
		std::shared_ptr<midware::NonvolatileDataHandler> m_po_nonvolatile_data_handler;
#endif /* USE_NVDH */
	};


}
#endif /* _MAIN_APPLICATION_HPP_ */
