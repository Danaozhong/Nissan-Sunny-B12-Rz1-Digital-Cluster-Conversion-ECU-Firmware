
#include "fuel_gauge_output.hpp"

#include "trace_if.h"


namespace app
{
    FuelGaugeOutput::FuelGaugeOutput(drivers::GenericDAC* p_dac, \
            const app::CharacteristicCurve<int32_t, int32_t> &o_fuel_output_characteristic, \
            int32_t i32_amplifying_factor, int32_t i32_aplifiying_offset)
      : m_p_dac(p_dac), 
        m_i32_current_fuel_percentage(0),
        m_i32_set_voltage_output(0),
        m_i32_set_voltage_dac(0),
        m_i32_amplifying_factor(i32_amplifying_factor), m_i32_aplifiying_offset(i32_aplifiying_offset),
        m_o_fuel_output_characteristic(o_fuel_output_characteristic)
    {
        TRACE_DECLARE_CONTEXT("FOUT");
        
    }

    int32_t FuelGaugeOutput::set_fuel_level(int32_t i32_fuel_level)
    {
        m_i32_set_voltage_output = m_o_fuel_output_characteristic.get_y(i32_fuel_level);

        TRACE_LOG("FOUT", LOGLEVEL_DEBUG, "Setting output to voltage %i\r\n", static_cast<int>(m_i32_set_voltage_output));

        // ...take the effects of the OpAmp into account.
        // the m_i32_amplifying_factor is given in x1000, therefore needs to multiply 1000 to balance out
        m_i32_set_voltage_dac = (m_i32_set_voltage_output - m_i32_aplifiying_offset) * 1000 / m_i32_amplifying_factor;

        // and then send the signal to the DAC.
        return m_p_dac->set_output_voltage(m_i32_set_voltage_dac);
    }

    int32_t FuelGaugeOutput::get_voltage_output() const
    {
        return m_i32_set_voltage_output;
    }

    int32_t FuelGaugeOutput::get_voltage_dac() const
    {
        return m_i32_set_voltage_dac;
    }
}
