
#include "fuel_gauge_output.hpp"

namespace app
{
	FuelGaugeOutput::FuelGaugeOutput(std::shared_ptr<drivers::GenericDAC> p_dac, \
			std::shared_ptr<app::CharacteristicCurve> p_fuel_output_characteristic, \
			double d_amplifying_factor, double d_aplifiying_offset)
	: m_p_dac(p_dac), m_p_fuel_output_characteristic(p_fuel_output_characteristic),
	  m_d_amplifying_factor(d_amplifying_factor), m_d_aplifiying_offset(d_aplifiying_offset)
	{}


	int32_t FuelGaugeOutput::set_fuel_level(double d_fuel_level)
	{
		if (d_fuel_level < 0.0 || d_fuel_level > 100.0)
		{
			return -1;
		}

		m_d_set_voltage = d_fuel_level;
		// Calculate the desired final voltage
		double d_final_voltage = m_p_fuel_output_characteristic->get_y(m_d_set_voltage);

		// ...take the effects of the OpAmp into account.
		m_d_set_voltage = (d_final_voltage - m_d_aplifiying_offset) / m_d_amplifying_factor;

		// and then send the signal to the DAC.
		return m_p_dac->set_output_voltage(m_d_set_voltage);
	}
}
