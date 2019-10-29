

#include "fuel_gauge_input.hpp"
#include "thread.hpp"
#include <functional> // for std::bind

// Ugly macros to allow debug logging
#ifndef FUEL_GAUGE_INPUT_ENABLE_LOGGING
#define FUEL_GAUGE_LOG(...)
#else
/* Compile with debug output */
#include "trace.h"
#define FUEL_GAUGE_LOG(...)   DEBUG_PRINTF(__VA_ARGS__)
#endif

namespace app
{

	VoltageDivider::VoltageDivider(double d_resistor_1, double d_supply_voltage)
		: m_d_resistor_1(d_resistor_1), m_d_supply_voltage(d_supply_voltage)
	{}

	double VoltageDivider::get_resistor_2_value(double d_resistor_2_voltage) const
	{
		// R2 = R1/(1 - U2/U) - R1
		return m_d_resistor_1 / (1.0 - d_resistor_2_voltage / m_d_supply_voltage) - m_d_resistor_1;
	}

	double VoltageDivider::get_resistor_2_voltage(double get_resistor_2_value) const
	{
		// currently noone cares
		return 0.0;
	}

	ParallelVoltageDivider::ParallelVoltageDivider(double d_resistor_1, double d_resistor_2_parallel, double d_supply_voltage)
	: m_d_resistor_1(d_resistor_1), m_d_supply_voltage(d_supply_voltage), m_d_resistor_2_parallel(d_resistor_2_parallel),
			m_voltage_divider(d_resistor_1, d_supply_voltage)
	{}

	double ParallelVoltageDivider::get_resistor_2_value(double d_resistor_2_voltage) const
	{
		double resistor_total = m_voltage_divider.get_resistor_2_value(d_resistor_2_voltage);
		// r*r2/(r2 - r) = r1
		return resistor_total * m_d_resistor_2_parallel / (m_d_resistor_2_parallel - resistor_total);
	}

	FuelGaugeInputFromADC::FuelGaugeInputFromADC(std::shared_ptr<drivers::GenericADC> p_adc, std::shared_ptr<app::CharacteristicCurve> p_fuel_input_characteristic)
	: m_p_adc(p_adc), m_p_fuel_input_characteristic(p_fuel_input_characteristic)
	{
		// start the data acquisition thread
		auto main_func = std::bind(&FuelGaugeInputFromADC::thread_main, this);
		std::thread o_data_acquisition_thread(main_func);

		// should be done nicer to allow cleanup, but currently threads will keep running forever
		o_data_acquisition_thread.detach();

	}

	void FuelGaugeInputFromADC::thread_main(void)
	{
		FUEL_GAUGE_LOG("Fuel Sensor Input Data Acquisition thread started!\r\n");
		// the voltage divider is supplied by 5V, and has a 220Ohm resistor on top,
		// and a 330Ohm resistor in parallel to the fuel gauge.
		ParallelVoltageDivider o_voltage_divider(220, 330, 5);

		// never terminate this thread.
		while(true)
		{
			// read from the ADC
			uint32_t u32_adc_value = m_p_adc->read_adc_value();
			FUEL_GAUGE_LOG("Current ADC value: %u\r\n", u32_adc_value);

			// covert to voltage
			double d_adc_pin_voltage = 3.3 * static_cast<double>(u32_adc_value) / static_cast<double>(m_p_adc->get_adc_max_value());

			// convert to resistor value of the fuel sensor
			m_d_last_read_resistor_value = o_voltage_divider.get_resistor_2_value(d_adc_pin_voltage);
			FUEL_GAUGE_LOG("Current calculated resistor value: %f\r\n", m_d_last_read_resistor_value);

			// find the percentage
			if (nullptr != m_p_fuel_input_characteristic)
			{
				m_d_read_fuel_percentage = m_p_fuel_input_characteristic->get_x(m_d_last_read_resistor_value);
				FUEL_GAUGE_LOG("Current calculated fuel input level: %f\r\n", m_d_read_fuel_percentage);

				// Send a signal that the fuel level has changed
				this->m_sig_fuel_level_changed(m_d_read_fuel_percentage);

			}
			std_ex::sleep_for(std::chrono::milliseconds(100));
		}

	}
}
