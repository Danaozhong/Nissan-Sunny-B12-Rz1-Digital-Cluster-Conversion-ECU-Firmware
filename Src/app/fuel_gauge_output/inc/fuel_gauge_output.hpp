#ifndef _FUEL_GAUGE_OUTPUT_HPP_
#define _FUEL_GAUGE_OUTPUT_HPP_

#include "generic_dac.hpp"
#include "lookup_table.hpp"


/* Enable this if you want to have additional log output for the fuel sensor acquisition module */
//#define FUEL_GAUGE_OUTPUT_ENABLE_LOGGING


namespace app
{
	/** Class to drive a fuel gauge via an DAC */
	class FuelGaugeOutput
	{
	public:
		FuelGaugeOutput(drivers::GenericDAC* p_dac, \
				app::CharacteristicCurve<int32_t, int32_t>* p_fuel_output_characteristic, \
				int32_t i32_amplifying_factor, int32_t i32_aplifiying_offset);

		int32_t set_fuel_level(int32_t i32_fuel_level);

		int32_t get_voltage_output() const;
		int32_t get_voltage_dac() const;
	private:
		/// The ADC used to retrieve data
		drivers::GenericDAC* m_p_dac;

		/// The current set fuel level percentage
		int32_t m_i32_current_fuel_percentage;

		/// the expected voltage level on the digital cluster output port
		uint32_t m_i32_set_voltage_output;

		/// the current voltage level set on the output DAC (before amplifying)
		int32_t m_i32_set_voltage_dac;

		/// If the DAC drives an opamp, this opamp might have an amplifing characteristic.
		const int32_t m_i32_amplifying_factor;

		/// If the DAC drives an opamp, this opamp might have an offset. Unit in volts
		const int32_t m_i32_aplifiying_offset;

		/* This map maps fuel levels to voltages */
		app::CharacteristicCurve<int32_t, int32_t>* m_p_fuel_output_characteristic;
	};
}

#endif /* _FUEL_GAUGE_OUTPUT_HPP_ */
