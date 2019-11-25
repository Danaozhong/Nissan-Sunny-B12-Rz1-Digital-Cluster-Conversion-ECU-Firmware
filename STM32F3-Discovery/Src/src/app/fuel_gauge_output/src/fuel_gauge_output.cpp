
#include "fuel_gauge_output.hpp"

// Ugly macros to allow debug logging
#ifndef FUEL_GAUGE_OUTPUT_ENABLE_LOGGING
#define FUEL_GAUGE_OUTPUT_LOG(...)
#else
/* Compile with debug output */
#include "trace.h"
#define FUEL_GAUGE_OUTPUT_LOG(...)   DEBUG_PRINTF(__VA_ARGS__)
#endif

namespace app
{
	FuelGaugeOutput::FuelGaugeOutput(std::shared_ptr<drivers::GenericDAC> p_dac, \
			std::shared_ptr<app::CharacteristicCurve<int32_t, double>> p_fuel_output_characteristic, \
			double d_amplifying_factor, double d_aplifiying_offset)
	: m_p_dac(p_dac), m_p_fuel_output_characteristic(p_fuel_output_characteristic),
	  m_d_amplifying_factor(d_amplifying_factor), m_d_aplifiying_offset(d_aplifiying_offset)
	{}


	int32_t FuelGaugeOutput::set_fuel_level(int32_t i32_fuel_level)
	{
		//if (i32_fuel_level < 0.0 || d_fuel_level > 100.0)
		//{
		//	return -1;
		//}

		//m_d_set_voltage = d_fuel_level;
		// Calculate the desired final voltage
		double d_final_voltage = m_p_fuel_output_characteristic->get_y(i32_fuel_level);

		FUEL_GAUGE_OUTPUT_LOG("Setting output to voltage %f\r\n", d_final_voltage);
		// ...take the effects of the OpAmp into account.
		m_d_set_voltage = (d_final_voltage - m_d_aplifiying_offset) / m_d_amplifying_factor;


		// and then send the signal to the DAC.
		return m_p_dac->set_output_voltage(m_d_set_voltage);
	}
}
