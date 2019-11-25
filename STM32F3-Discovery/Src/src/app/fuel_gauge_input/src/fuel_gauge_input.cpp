

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

	FuelGaugeInputFromADC::FuelGaugeInputFromADC(std::shared_ptr<drivers::GenericADC> p_adc,
			std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> p_fuel_input_characteristic)
	: m_p_adc(p_adc), m_p_fuel_input_characteristic(p_fuel_input_characteristic),
	  m_u32_buffer_counter(0u), m_bo_initialized(false), m_u32_invalid_read_counter(0u)
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

			// convert to resistor value of the fuel sensor (in mOhm)
			int32_t i32_last_read_resistor_value = static_cast<int32_t>(1000.0 * o_voltage_divider.get_resistor_2_value(d_adc_pin_voltage));
			FUEL_GAUGE_LOG("Current calculated resistor value: %i mOhm\r\n", i32_last_read_resistor_value);

			// find the percentage
			if (nullptr != m_p_fuel_input_characteristic)
			{

				int32_t i32_read_fuel_percentage = m_p_fuel_input_characteristic->get_x(i32_last_read_resistor_value);

				/* On the first read, initialize the buffer with the read values */
				if (false == m_bo_initialized)
				{
					m_bo_initialized = true;
					for (uint32_t u32i = 0; u32i < FUEL_GAUGE_INPUT_AVERAGING_SIZE; ++u32i)
					{
						m_ai32_last_read_fuel_percentages[u32i] = i32_read_fuel_percentage;
					}
				}

				/* Check if the deviation is more than 10% */
				if (std::abs(get_average_fuel_percentage() - i32_read_fuel_percentage) < 2000)
				{
					/* only take the value into account if it differs less than 10% */
					m_ai32_last_read_fuel_percentages[m_u32_buffer_counter] = i32_read_fuel_percentage;
					m_u32_buffer_counter++;
					if (m_u32_buffer_counter >= FUEL_GAUGE_INPUT_AVERAGING_SIZE)
					{
						m_u32_buffer_counter = 0;
					}

					// Send a signal that the fuel level has changed
					const int32_t i32_read_fuel_percentage = get_average_fuel_percentage();
					FUEL_GAUGE_LOG("Current calculated fuel input level: %i\r\n", i32_read_fuel_percentage);
					this->m_sig_fuel_level_changed(i32_read_fuel_percentage);

					// since we have read a valid value, reset the invalid data counter
					m_u32_invalid_read_counter = 0;
				}
				else
				{
					m_u32_invalid_read_counter++;

					/* If we keep reading nonsense, throw everything away and start again */
					if (m_u32_invalid_read_counter > 10)
					{
						FUEL_GAUGE_LOG("Read fuel input data did not make sense, starting again.\r\n");
						m_bo_initialized = false;
					}
				}



			}
			std_ex::sleep_for(std::chrono::milliseconds(200));
		}

	}

	int32_t FuelGaugeInputFromADC::get_average_fuel_percentage() const
	{
		int64_t i64_avg_value = 0ull;
		for (uint32_t u32i = 0; u32i < FUEL_GAUGE_INPUT_AVERAGING_SIZE; ++u32i)
		{
			i64_avg_value += static_cast<int64_t>(m_ai32_last_read_fuel_percentages[u32i]);
		}
		return static_cast<int32_t>(i64_avg_value / FUEL_GAUGE_INPUT_AVERAGING_SIZE);
	}
}
