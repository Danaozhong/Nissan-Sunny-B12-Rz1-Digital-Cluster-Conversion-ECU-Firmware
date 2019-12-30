#ifndef _MAIN_APPLICATION_HPP_
#define _MAIN_APPLICATION_HPP_

/* Own headers */
#include "fuel_gauge_input.hpp"
#include "fuel_gauge_output.hpp"
#include "lookup_table.hpp"
#include "speed_sensor_converter.hpp"
#include "main.h"


#include "stm32_dac.hpp"
#include "stm32_adc.hpp"
#include "stm32_uart.hpp"
#include "stm32_pwm.hpp"

#include "os_console.hpp"
#include "ex_thread.hpp"


namespace app
{
	class MainApplication
	{
	public:
		/** Constructor */
		MainApplication();

		/** Callback used when the fuel sensor input has detected an input level change */
		void fuel_sensor_input_received(int32_t i32_value);

		/** Singleton accessor */
		static MainApplication& get();


		std::shared_ptr<SpeedSensorConverter> get_speed_sensor_converter() const;

    private:
        // prevent copying
        MainApplication(MainApplication &other) = delete;

        int32_t initilialize_speed_converter();
        int32_t initialize_fuel_level_converter();

		std::shared_ptr<drivers::GenericADC> m_p_adc;
		std::shared_ptr<drivers::GenericDAC> m_p_dac;

		/// the PWM output used for the speed sensor
		std::shared_ptr<drivers::GenericPWM> m_p_pwm;

		std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> m_p_o_fuel_gauge_input_characteristic;
		std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> m_p_o_fuel_gauge_output_characteristic;

		app::FuelGaugeInputFromADC* m_p_o_fuel_gauge_input;
		std::shared_ptr<app::FuelGaugeOutput> m_p_o_fuel_gauge_output;

		std::shared_ptr<SpeedSensorConverter> m_po_speed_sensor_converter;
	};


}
#endif /* _MAIN_APPLICATION_HPP_ */
