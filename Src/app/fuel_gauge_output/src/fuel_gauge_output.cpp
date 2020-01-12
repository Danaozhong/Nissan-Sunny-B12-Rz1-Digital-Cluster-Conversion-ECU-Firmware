
#include "fuel_gauge_output.hpp"

// Ugly macros to allow debug logging
#ifndef FUEL_GAUGE_OUTPUT_ENABLE_LOGGING
#define FUEL_GAUGE_OUTPUT_LOG(...)
#else
/* Compile with debug output */
#include "trace_if.h"
#define FUEL_GAUGE_OUTPUT_LOG(...)   DEBUG_PRINTF(__VA_ARGS__)
#endif

namespace app
{
	FuelGaugeOutput::FuelGaugeOutput(std::shared_ptr<drivers::GenericDAC> p_dac, \
			std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> p_fuel_output_characteristic, \
			int32_t i32_amplifying_factor, int32_t i32_aplifiying_offset)
	: m_p_dac(p_dac), m_p_fuel_output_characteristic(p_fuel_output_characteristic),
	  m_i32_amplifying_factor(i32_amplifying_factor), m_i32_aplifiying_offset(i32_aplifiying_offset)
	{}


	int32_t FuelGaugeOutput::set_fuel_level(int32_t i32_fuel_level)
	{
		//if (i32_fuel_level < 0.0 || d_fuel_level > 100.0)
		//{
		//	return -1;
		//}

		//m_d_set_voltage = d_fuel_level;
		// Calculate the desired final voltage
		int32_t i32_final_voltage = m_p_fuel_output_characteristic->get_y(i32_fuel_level);

		FUEL_GAUGE_OUTPUT_LOG("Setting output to voltage %s\r\n", i32_final_voltage);
		// ...take the effects of the OpAmp into account.
		m_i32_set_voltage = (i32_final_voltage - m_i32_aplifiying_offset) / m_i32_amplifying_factor;


		// and then send the signal to the DAC.
		return m_p_dac->set_output_voltage(m_i32_set_voltage);
	}
}
