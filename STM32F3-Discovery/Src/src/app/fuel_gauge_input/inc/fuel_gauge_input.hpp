#ifndef _FUEL_GAUGE_INPUT_HPP_
#define _FUEL_GAUGE_INPUT_HPP_

#include <memory>
#include "generic_adc.hpp"
#include "lookup_table.hpp"


/* Enable this if you want to have additional log output for the fuel sensor acquisition module */
#define FUEL_GAUGE_INPUT_ENABLE_LOGGING


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
		FuelGaugeInputFromADC(std::shared_ptr<drivers::GenericADC> p_adc, std::shared_ptr<app::CharacteristicCurve> p_fuel_input_characteristic);


	private:
		void thread_main(void);

		/// The ADC used to retrieve data
		std::shared_ptr<drivers::GenericADC> m_p_adc;

		double m_d_read_fuel_percentage;
		double m_d_last_read_resistor_value;

		std::shared_ptr<app::CharacteristicCurve> m_p_fuel_input_characteristic;


	};


}

#endif /* _FUEL_GAUGE_INPUT_HPP_ */
