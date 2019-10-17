#ifndef _FUEL_GAUGE_OUTPUT_HPP_
#define _FUEL_GAUGE_OUTPUT_HPP_

#include <memory>
#include "generic_dac.hpp"
#include "lookup_table.hpp"


/* Enable this if you want to have additional log output for the fuel sensor acquisition module */
#define FUEL_GAUGE_OUTPUT_ENABLE_LOGGING


namespace app
{
	/** Class to drive a fuel gauge via an DAC */
	class FuelGaugeOutput
	{
	public:
		FuelGaugeOutput(std::shared_ptr<drivers::GenericDAC> p_dac, \
				std::shared_ptr<app::CharacteristicCurve> p_fuel_output_characteristic, \
				double d_amplifying_factor, double d_aplifiying_offset);

		int32_t set_fuel_level(double d_fuel_level);

	private:
		/// The ADC used to retrieve data
		std::shared_ptr<drivers::GenericDAC> m_p_dac;

		/// The current set fuel level percentage
		double m_d_current_fuel_percentage;

		/// the current voltage level set on the output
		double m_d_set_voltage;

		/// If the DAC drives an opamp, this opamp might have an amplifing characteristic.
		const double m_d_amplifying_factor;

		/// If the DAC drives an opamp, this opamp might have an offset. Unit in volts
		const double m_d_aplifiying_offset;

		/* This map maps fuel levels to voltages */
		std::shared_ptr<app::CharacteristicCurve> m_p_fuel_output_characteristic;
	};
}

#endif /* _FUEL_GAUGE_OUTPUT_HPP_ */
