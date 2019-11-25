#ifndef _FUEL_GAUGE_INPUT_HPP_
#define _FUEL_GAUGE_INPUT_HPP_

#include <memory>
#include "generic_adc.hpp"
#include "lookup_table.hpp"
#include "event_handler.h"


/* Enable this if you want to have additional log output for the fuel sensor acquisition module */
#define FUEL_GAUGE_INPUT_ENABLE_LOGGING


#define FUEL_GAUGE_INPUT_AVERAGING_SIZE   (7u)

namespace app
{
	/** Helper class to calculate the voltage / resistor values at a voltage divider */
	class VoltageDivider
	{
	public:
		VoltageDivider(double d_resistor_1, double d_supply_voltage);

		double get_resistor_2_value(double d_resistor_2_voltage) const;
		double get_resistor_2_voltage(double get_resistor_2_value) const;
	private:
		double m_d_resistor_1;
		double m_d_supply_voltage;
	};

	class ParallelVoltageDivider
	{
	public:
		ParallelVoltageDivider(double d_resistor_1, double d_resistor_2_parallel, double d_supply_voltage);

		double get_resistor_2_value(double d_resistor_2_voltage) const;

	private:
		VoltageDivider m_voltage_divider;
		double m_d_resistor_1;
		double m_d_resistor_2_parallel;
		double m_d_supply_voltage;
	};

	class FuelGaugeInputFromADC
	{
	public:
		FuelGaugeInputFromADC(std::shared_ptr<drivers::GenericADC> p_adc,
				std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> p_fuel_input_characteristic);

		/// Signal triggered when a new value from the fuel sensor was retrieved
		boost::signals2::signal<int32_t> m_sig_fuel_level_changed;
	private:
		void thread_main(void);


		int32_t get_average_fuel_percentage() const;

		/// The ADC used to retrieve data
		std::shared_ptr<drivers::GenericADC> m_p_adc;

		int32_t m_ai32_last_read_fuel_percentages[FUEL_GAUGE_INPUT_AVERAGING_SIZE]; /* in percent * 100 */
		uint32_t m_u32_buffer_counter;

		bool m_bo_initialized;
		uint32_t m_u32_invalid_read_counter;

		//uint32_t m_u32_read_fuel_percentage;
		//uint32_t m_u32_last_read_resistor_value; /* in ohm * 1000 */

		std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> m_p_fuel_input_characteristic;


	};


}

#endif /* _FUEL_GAUGE_INPUT_HPP_ */
