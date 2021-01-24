
#include "fuel_gauge_input.hpp"
#include "excp_handler_if.h"
#include <functional> // for std::bind
#include "os_console.hpp"
#include "trace_if.h"
#include "util_algorithms.hpp"

// allow up to 10% deviation of the values
#define FUEL_GAUGE_INPUT_READINGS_MAX_DISTANCE 1000 // % * 100
#define FUEL_GAUGE_INPUT_READINGS_INVALID_DATA_DISTANCE 2500 // % * 100

#define FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_UP 500 // % * 100
#define FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_DOWN 500 // % * 100

#define FUEL_GAUGE_INPUT_PERCENTAGE_UPDATE_THRESHOLD 50 // the threshold in 100*%, when the fuel value should be updated

#define FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD  (30) // seconds

using namespace midware;

namespace app
{
	VoltageDivider::VoltageDivider(uint32_t u32_resistor_1, int32_t i32_supply_voltage)
		: m_u32_resistor_1(u32_resistor_1), m_i32_supply_voltage(i32_supply_voltage)
	{}

	uint32_t VoltageDivider::get_resistor_2_value(int32_t i32_resistor_2_voltage) const
	{
		// R2 = R1/(U - U2) / U - R1
		return (m_u32_resistor_1 * m_i32_supply_voltage) / (m_i32_supply_voltage - i32_resistor_2_voltage ) - m_u32_resistor_1;
	}

	int32_t VoltageDivider::get_resistor_2_voltage(uint32_t u32_resistor_2_value) const
	{
		// currently noone cares
		return 0;
	}

	int32_t VoltageDivider::get_supply_voltage() const
	{
	    return m_i32_supply_voltage;
	}

	ParallelVoltageDivider::ParallelVoltageDivider(uint32_t d_resistor_1, uint32_t d_resistor_2_parallel, int32_t d_supply_voltage)
	: m_u32_resistor_1(d_resistor_1), m_i32_supply_voltage(d_supply_voltage), m_u32_resistor_2_parallel(d_resistor_2_parallel),
			m_voltage_divider(d_resistor_1, d_supply_voltage)
	{}

	uint32_t ParallelVoltageDivider::get_resistor_2_value(int32_t i32_resistor_2_voltage) const
	{
		uint32_t resistor_total = m_voltage_divider.get_resistor_2_value(i32_resistor_2_voltage);
		// r*r2/(r2 - r) = r1
		return resistor_total * m_u32_resistor_2_parallel / (m_u32_resistor_2_parallel - resistor_total);
	}


	FuelGaugeInputFromADC::FuelGaugeInputFromADC(drivers::GenericADC* p_adc,
			const app::CharacteristicCurve<int32_t, int32_t>& o_fuel_input_characteristic)
	 : m_p_adc(p_adc), m_o_fuel_input_characteristic(o_fuel_input_characteristic),
       m_en_state(FuelGaugeInputSMStarting),
       m_u32_input_unexpected_higher_error_counter(0u),
       m_u32_input_unexpected_lower_error_counter(0u),
      m_o_voltage_divider(100000, 3300), // 100 Ohm (value representation is in mOhm), 3V3 supply
      m_i32_adc_pin_voltage(0),
      m_i32_fuel_sensor_resistor_value(0)
	{
	    TRACE_DECLARE_CONTEXT("FLIN");
	}

	FuelGaugeInputFromADC::~FuelGaugeInputFromADC()
	{

	    // disconnecting the signal handler should not be necessary, will we done automatically during deletion
	}


    void FuelGaugeInputFromADC::cycle_100ms()
    {
        uint32_t u32_adc_value = m_p_adc->read_adc_value();
        TRACE_LOG("FLIN", LOGLEVEL_DEBUG, "Current ADC value: %u\r\n", u32_adc_value);

        // covert to voltage
        m_i32_adc_pin_voltage = (m_o_voltage_divider.get_supply_voltage() * u32_adc_value) / m_p_adc->get_adc_max_value();
        TRACE_LOG("FLIN", LOGLEVEL_DEBUG, "Current ADC voltage: %i.%iV\r\n", m_i32_adc_pin_voltage / 1000, m_i32_adc_pin_voltage % 1000);

        // convert to resistor value of the fuel sensor (in mOhm)
        m_i32_fuel_sensor_resistor_value = m_o_voltage_divider.get_resistor_2_value(m_i32_adc_pin_voltage);
        TRACE_LOG("FLIN", LOGLEVEL_DEBUG, "Current resistor value: %i mOhm\r\n", m_i32_fuel_sensor_resistor_value);

        // find the percentage
        const int32_t i32_read_fuel_percentage = m_o_fuel_input_characteristic.get_x(m_i32_fuel_sensor_resistor_value);
        TRACE_LOG("FLIN", LOGLEVEL_DEBUG,"Current fuel input level: %i%%*100\r\n", i32_read_fuel_percentage);

        m_ai32_raw_last_read_fuel_percentages.push_back(i32_read_fuel_percentage);

        bool bo_new_averaged_value_reached = false;

        if (m_ai32_raw_last_read_fuel_percentages.size() >= FUEL_GAUGE_INPUT_AVERAGING_SIZE)
        {
            // calculate an average by removing the outliers
            std::vector<int32_t> copied_data = m_ai32_raw_last_read_fuel_percentages;
            const auto averaged_value = Algorithms::filter_outliers_by_max_deviation<int32_t>(copied_data,
                    3u, /* take at least 3 values into account */
                    FUEL_GAUGE_INPUT_READINGS_MAX_DISTANCE);

            int i_average_value = static_cast<int>(std::get<0>(averaged_value) / 100);
            TRACE_LOG("FLIN", LOGLEVEL_INFO,"newly averaged fuel value calculated: %i%%, min/max diff%i%%\r\n", i_average_value, std::get<1>(averaged_value) / 100);

            // check if the value makes sense. Only allow up to x% deviation in the values
            if (std::get<1>(averaged_value) <= FUEL_GAUGE_INPUT_READINGS_INVALID_DATA_DISTANCE)
            {
                // calculate the weight. no diff means maximum weight. Take the square to weight high variance values little
                int32_t weight = 10000 - std::get<1>(averaged_value); // 100% - min/max diff
                weight = weight*weight;

                m_ai32_averaged_fuel_readings_buffer.push_back(std::pair<int32_t, int32_t>(std::get<0>(averaged_value), weight));

                // limit the number of averages
                if (m_ai32_averaged_fuel_readings_buffer.size() >= FUEL_GAUGE_INPUT_NUM_OF_AVERAGES)
                {
                    m_ai32_averaged_fuel_readings_buffer.erase(m_ai32_averaged_fuel_readings_buffer.begin());
                }
                bo_new_averaged_value_reached = true;

                m_ai32_raw_last_read_fuel_percentages.clear();
            }
            else
            {
                // the data doesn't make sense - keep throwing away old readings, until it makes sense again.
                TRACE_LOG("FLIN", LOGLEVEL_ERROR,"The raw readings doesn't make sense, throwing away old reading...\r\n");
                m_ai32_raw_last_read_fuel_percentages.erase(m_ai32_raw_last_read_fuel_percentages.begin());
            }
        }

        // run the state machine
        switch (m_en_state)
        {
        case FuelGaugeInputSMStarting:
            if(m_ai32_averaged_fuel_readings_buffer.size() == 0)
            {
                /* this happens within the first second of calibration. There is not yet an averaged value available. */
                std::vector<int32_t> copied_data = m_ai32_raw_last_read_fuel_percentages;
                const auto averaged_value = Algorithms::filter_outliers_by_max_deviation<int32_t>(copied_data,
                        3u, /* take at least 3 values into account */
                        FUEL_GAUGE_INPUT_READINGS_MAX_DISTANCE);

                // immediately update the fuel output value to have at least some sort of indication
                set_fuel_sensor_value(std::get<0>(averaged_value));

                TRACE_LOG("FLIN", LOGLEVEL_INFO, "updating initial sensor input: %i%%, min/max diff%i%%\r\n", std::get<0>(averaged_value) / 100, std::get<1>(averaged_value) / 100);
            }
            else
            {
                int32_t averaged_weighted_fuel_input = static_cast<int32_t>(Algorithms::get_weighted_average<std::vector<std::pair<int32_t, int32_t> >::iterator, int64_t>
                    (m_ai32_averaged_fuel_readings_buffer.begin(), m_ai32_averaged_fuel_readings_buffer.end()));
                set_fuel_sensor_value(averaged_weighted_fuel_input);
                TRACE_LOG("FLIN", LOGLEVEL_INFO, "updating initial sensor input on weighted data: %i%%\r\n", averaged_weighted_fuel_input / 100);
            }

            if (m_ai32_averaged_fuel_readings_buffer.size() >= 3)
            {
                // after three seconds of calibrating, jump to the normal state.
                m_en_state = FuelGaugeInputSMNormalOperation;
                m_u32_input_unexpected_higher_error_counter = 0;
                m_u32_input_unexpected_lower_error_counter = 0;
            }
        default:
            break;

            // all other state are only processed once a new value for the state machine was reached.
        }

        if (bo_new_averaged_value_reached)
        {
            switch(m_en_state)
            {
            case FuelGaugeInputSMNormalOperation:
                {
                    // get the current averaged, weighted fuel sensor value
                    int32_t averaged_weighted_fuel_input = static_cast<int32_t>(Algorithms::get_weighted_average<std::vector<std::pair<int32_t, int32_t> >::iterator, int64_t>
                    (m_ai32_averaged_fuel_readings_buffer.begin(), m_ai32_averaged_fuel_readings_buffer.end()));

                    // check if the fuel value should be updated
                    if (get_fuel_sensor_value() < averaged_weighted_fuel_input + FUEL_GAUGE_INPUT_PERCENTAGE_UPDATE_THRESHOLD && get_fuel_sensor_value() > averaged_weighted_fuel_input)
                    {
                        // there is a slight distance between the currently set fuel sensor value and what is expected, update fuel sensor value
                        TRACE_LOG("FLIN", LOGLEVEL_INFO,"Fuel reading is slightly reduced, set to %i\r\n", averaged_weighted_fuel_input);
                        set_fuel_sensor_value(averaged_weighted_fuel_input);
                    }

                    //  compare to large differences, and increase the error counters accordingly
                    if (get_fuel_sensor_value() + FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_UP < averaged_weighted_fuel_input)
                    {
                        // measured fuel value is >> larger than what is currently set, we might have a refill
                        m_u32_input_unexpected_higher_error_counter += 1;
                        TRACE_LOG("FLIN", LOGLEVEL_WARNING,"Fuel reading is unexpected high (error counter %u, current set value: %i, current read value: %i)\r\n",
                                m_u32_input_unexpected_higher_error_counter,
                                get_fuel_sensor_value(),
                                averaged_weighted_fuel_input);
                    }
                    else if (get_fuel_sensor_value() > averaged_weighted_fuel_input + FUEL_GAUGE_INPUT_PERCENTAGE_THRESHOLD_DOWN)
                    {
                        // measured fuel value is << less than what is currently set, either the tank has a hole, or the SW has a bug.
                        m_u32_input_unexpected_lower_error_counter += 1;
                        TRACE_LOG("FLIN", LOGLEVEL_ERROR,"Fuel reading is unexpected low (error counter %u, current set value: %i, current read value: %i)\r\n",
                                m_u32_input_unexpected_lower_error_counter,
                                get_fuel_sensor_value(),
                                averaged_weighted_fuel_input);
                    }
                    else
                    {
                        // current read value is roughly the same as before, reduce the error counters
                        if (m_u32_input_unexpected_higher_error_counter >= 2)
                        {
                            m_u32_input_unexpected_higher_error_counter -= 2;
                        }
                        else
                        {
                            m_u32_input_unexpected_higher_error_counter = 0;
                        }

                        if (m_u32_input_unexpected_lower_error_counter >= 2)
                        {
                            m_u32_input_unexpected_lower_error_counter -= 2;
                        }
                        else
                        {
                            m_u32_input_unexpected_lower_error_counter = 0;
                        }
                    }

                    // check if the error counters have exceeded their thresholds
                    if (m_u32_input_unexpected_higher_error_counter > FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD)
                    {
                        // refill detected!
                        m_en_state = FuelGaugeInputSMRefillDetected;
                    }
                    if (m_u32_input_unexpected_lower_error_counter > FUEL_GAUGE_INPUT_ERROR_COUNTER_THRESHOLD)
                    {
                        // sudden fuel drop detected, need to recalibrate
                        m_en_state = FuelGaugeInputSMErrorRecovery;
                    }
                    break;
                }
            case FuelGaugeInputSMRefillDetected:
                {
                    // the read fuel value was consistently much larger than what is currently set, probably the car was refilled
                    TRACE_LOG("FLIN", LOGLEVEL_WARNING, "Refill detected, start recalibration...\r\n");

                    // clear all error counters
                    m_u32_input_unexpected_lower_error_counter = 0;
                    m_u32_input_unexpected_lower_error_counter = 0;

                    // and start a new calibration
                    m_en_state = FuelGaugeInputSMStarting;
                    break;
                }
            case FuelGaugeInputSMErrorRecovery:
                {
                    // the read fuel value was consistently much larger than what is currently set, probably the car was refilled
                    TRACE_LOG("FLIN", LOGLEVEL_ERROR, "Too low fuel value detected, start recalibration...\r\n");

                    // clear all error counters
                    m_u32_input_unexpected_lower_error_counter = 0;
                    m_u32_input_unexpected_lower_error_counter = 0;

                    // and start a new calibration
                    m_en_state = FuelGaugeInputSMStarting;
                    break;
                }
            }
        }
    }

    int32_t FuelGaugeInputFromADC::get_fuel_sensor_value() const
    {
        return m_i32_current_fuel_value;
    }

    int32_t FuelGaugeInputFromADC::get_current_averaged_fuel_sensor_value() const
    {
        if (m_ai32_averaged_fuel_readings_buffer.size() > 0)
        {
            return std::get<0>(m_ai32_averaged_fuel_readings_buffer.back());
        }
        return 0;
    }

    int32_t FuelGaugeInputFromADC::get_current_raw_fuel_sensor_value() const
    {
        if (m_ai32_raw_last_read_fuel_percentages.size() > 0)
        {
            return m_ai32_raw_last_read_fuel_percentages.back();
        }
    }

    int32_t FuelGaugeInputFromADC::get_adc_voltage() const
    {
        return m_i32_adc_pin_voltage;
    }

    int32_t FuelGaugeInputFromADC::get_fuel_sensor_resistor_value() const
    {
        return m_i32_fuel_sensor_resistor_value;
    }

    void FuelGaugeInputFromADC::set_fuel_sensor_value(int32_t i32_fuel_value)
    {
        m_i32_current_fuel_value = i32_fuel_value;
        this->m_sig_fuel_level_changed(i32_fuel_value);
    }


}
