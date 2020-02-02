#ifndef _MAIN_APPLICATION_HPP_
#define _MAIN_APPLICATION_HPP_

/* Own headers */
#include "fuel_gauge_input.hpp"
#include "fuel_gauge_output.hpp"
#include "lookup_table.hpp"
#include "speed_sensor_converter.hpp"

#include "os_console.hpp"
#include "excp_handler.hpp"
#include "main.h"


#include "stm32_dac.hpp"
#include "stm32_adc.hpp"
#include "stm32_uart.hpp"
#include "stm32_pwm.hpp"
#include "stm32_pwm_ic.hpp"

#include "nonvolatile_data_handler.hpp"

#include "os_console.hpp"
#include "ex_thread.hpp"
#ifdef USE_TRACE
#include "trace.hpp"
#endif

namespace app
{
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

		std::shared_ptr<SpeedSensorConverter> get_speed_sensor_converter() const;

		OSServices::OSConsole* get_os_console();


        /** Use this to select the mode in which the speed signal is sent out to the cluster.
         * use OUTPUT_MODE_CONVERSION to derive the speed signal from the input speed sensor,
         * alternatively; use OUTPUT_MODE_MANUAL to manually configure a speed value. */
        void set_fuel_gauge_output_mode(FuelGaugeOutputMode en_speed_output_mode);

        /** When the speed sensor conversion is in manual mode, use this function to set the manual
         * speed value.
         * \param[in] i32_speed_in_mph  The velocity in meters / hour.
         */
        int32_t set_manual_fuel_gauge_output_value(int32_t _i32_fuel_gauge_output_value);

    //private:
        // prevent copying
        MainApplication(MainApplication &other) = delete;

        int32_t init_speed_converter();

        int32_t deinit_speed_converter();

        int32_t init_fuel_level_converter();
        int32_t deinit_fuel_level_converter();

        const std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> get_fuel_input_characterics() const;
        const std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> get_fuel_output_characterics() const;


        drivers::GenericUART* m_p_uart;
        OSServices::OSConsole* m_po_os_console;
        std::shared_ptr<OSServices::OSConsoleGenericIOInterface> m_po_os_io_interface;

        midware::ExceptionHandler* m_po_exception_handler;
#ifdef USE_TRACE
        midware::Trace* m_po_trace;
#endif

		std::shared_ptr<drivers::GenericADC> m_p_adc;
		std::shared_ptr<drivers::GenericDAC> m_p_dac;

		/// the PWM output used to simulate the JDM speed sensor to the cluster
		std::shared_ptr<drivers::GenericPWM> m_p_pwm;

		/// The PWM Input Capture driver to read the PWM from the vehicle's speed sensor
		std::shared_ptr<drivers::GenericPWM_IC> m_p_pwm_ic;

		std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> m_p_o_fuel_gauge_input_characteristic;
		std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> m_p_o_fuel_gauge_output_characteristic;

		app::FuelGaugeInputFromADC* m_p_o_fuel_gauge_input;
		std::unique_ptr<app::FuelGaugeOutput> m_p_o_fuel_gauge_output;

		FuelGaugeOutputMode m_en_fuel_gauge_output_mode;
		int32_t m_i32_fuel_sensor_read_value;
		int32_t m_i32_fuel_gauge_output_manual_value;

		std::unique_ptr<SpeedSensorConverter> m_po_speed_sensor_converter;

		std::unique_ptr<midware::NonvolatileDataHandler> m_po_nonvolatile_data_handler;
	};


}
#endif /* _MAIN_APPLICATION_HPP_ */
