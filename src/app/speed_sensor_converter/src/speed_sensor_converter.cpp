#include "speed_sensor_converter.hpp"
#include "os_console.hpp"
#include "excp_handler_if.h"
#include "trace_if.h"
#include <algorithm>

#include <cmath>

// Specifies the minimum speed difference to the already displayed speed value, in order to update the displayed speed. Unit is m/h.
#define SPEED_SENSOR_UPDATE_THRESHOLD  (500)  // 0.5km/h

app::SpeedSensorConverter* po_speed_sensor_converter_instance = nullptr;
void speed_sensor_pwm_input_capture_callback(drivers::GenericPWM_IC* p_ic_obj, uint32_t u32_read_frequency, uint32_t u32_read_duty_cyclce)
{
    /* One could do a mapping here, but let's be simple, since we are only using one PWM IC for now. */
    if (nullptr != po_speed_sensor_converter_instance)
    {
        po_speed_sensor_converter_instance->pwm_input_capture_callback(u32_read_frequency, u32_read_duty_cyclce);
    }
}

namespace app
{
    namespace SpeedSensorConverterHelper
    {
        SpeedInputCaptureConfiguration pwm_ic_state_machine[SPEED_INPUT_CAPTURE_NUM_OF_MODES] =
        {
            {
                .u32_vehicle_speed_mph_lower_threshold = 0, /* not used */
                .en_lower_capture_mode =SPEED_INPUT_CAPTURE_NUM_OF_MODES,
                .u32_vehicle_speed_mph_upper_threshold = 50*1000, /* at 50km/h, switch to high speed mode. */
                .en_higher_capture_mode = SPEED_INPUT_CAPTURE_MODE_HIGH_SPEED,
                .u16_pwm_ic_prescaler = 2048 - 1/* will deliver good results from 1km/h to about 80km/h */
            },
            {
                .u32_vehicle_speed_mph_lower_threshold = 40*1000, /* dropping below 40km/h, activate low speed mode */
                .en_lower_capture_mode =SPEED_INPUT_CAPTURE_MODE_LOW_SPEED,
                .u32_vehicle_speed_mph_upper_threshold = 0, /* not used */
                .en_higher_capture_mode = SPEED_INPUT_CAPTURE_NUM_OF_MODES,
                .u16_pwm_ic_prescaler = 128 - 1 /* will deliver good results from 20km/h to about >300km/h */
            }
        };
    }


    SpeedSensorConverter::SpeedSensorConverter(drivers::GenericPWM* p_output_pwm,
                drivers::GenericPWM_IC* p_output_pwm_input_capture,
                uint32_t u32_input_pulses_per_kmph_mHz,
                uint32_t u32_output_pulses_per_kmph_mHz)
    : m_p_output_pwm(p_output_pwm), m_p_output_pwm_input_capture(p_output_pwm_input_capture),
      m_en_speed_sensor_variant(SPEED_SENSOR_9000RPM),
      m_en_current_speed_output_mode(OUTPUT_MODE_CONVERSION),
      m_i32_manual_speed(0), 
      m_u32_current_vehicle_speed_mph(0u),
      m_u8_input_array_position(0u),
      m_u32_new_output_frequency_mHz(0u),
      m_u32_input_pulses_per_kmph_mHz(u32_input_pulses_per_kmph_mHz),
      m_u32_output_pulses_per_kmph_mHz(u32_output_pulses_per_kmph_mHz),
      m_u32_num_of_pwm_captures(0u), m_u32_num_of_processed_pwm_captures(0u),
      m_maximum_reading_validity(std::chrono::milliseconds(800)),
      m_en_pwm_ic_state_machine_state(SPEED_INPUT_CAPTURE_MODE_LOW_SPEED)
    {
        static_assert(SPEED_SENSOR_READINGS_BUFFER_LENGTH > 0, "Buffer length for the sensor data readings must be at least 1.");

        TRACE_DECLARE_CONTEXT("SPD");

        // Create a dummy replay curve
        std::pair<int32_t, int32_t> replay_curve_data[] =
        {
            std::make_pair(0, 0),
            std::make_pair(2000, 180),
            std::make_pair(3000, 150),
            std::make_pair(9000, 160),
            std::make_pair(11000, 160),
            std::make_pair(14000, 0),
            std::make_pair(15000, 25),
            std::make_pair(16000, 60),
            std::make_pair(17000, 25),
            std::make_pair(18000, 60),
            std::make_pair(22000, 0),
            std::make_pair(22000, 0),
            std::make_pair(25000, 0),
            std::make_pair(28000, 40),
            std::make_pair(35000, 55)
        };
        
        clear_measured_data();
        
        app::CharacteristicCurve<int32_t, int32_t> replay_dataset(replay_curve_data, sizeof(replay_curve_data) / sizeof(replay_curve_data[0]));

        m_o_replay_curve.load_data(replay_dataset);

        po_speed_sensor_converter_instance = this;
        if (nullptr != m_p_output_pwm_input_capture)
        {
            m_p_output_pwm_input_capture->set_capture_callback(&speed_sensor_pwm_input_capture_callback);
        }
        // set the output to 0
        cycle();
    }

    SpeedSensorConverter::~SpeedSensorConverter()
    {
        if (nullptr != m_p_output_pwm_input_capture)
        {
            m_p_output_pwm_input_capture->set_capture_callback(nullptr);
        }
        po_speed_sensor_converter_instance = nullptr;
        // everything else is cleaned up automatically
    }

    /** Use this to select the mode in which the speed signal is sent out to the cluster.
     * use OUTPUT_MODE_CONVERSION to derive the speed signal from the input speed sensor,
     * alternatively; use OUTPUT_MODE_MANUAL to manually configure a speed value. */
    void SpeedSensorConverter::set_speed_output_mode(SpeedOutputMode en_speed_output_mode)
    {
        m_en_current_speed_output_mode = en_speed_output_mode;
        m_o_replay_curve.stop();

        if (OUTPUT_MODE_REPLAY == m_en_current_speed_output_mode)
        {
            m_o_replay_curve.play();
        }
    }

    /** When the speed sensor conversion is in manual mode, use this function to set the manual
     * speed value.
     * \param[in] i32_speed_in_mph  The velocity in meters / hour.
     */
    int32_t SpeedSensorConverter::set_manual_speed(int32_t i32_speed_in_mph)
    {
        // speed limit protection, make sure to not damage the cluster
        if (true == check_if_speed_is_valid(i32_speed_in_mph / 1000))
        {
            m_i32_manual_speed = i32_speed_in_mph / 1000;
            return OSServices::ERROR_CODE_SUCCESS;
        }
        return OSServices::ERROR_CODE_PARAMETER_WRONG;
    }

    int32_t SpeedSensorConverter::get_current_displayed_speed() const
    {
        if (OUTPUT_MODE_CONVERSION == m_en_current_speed_output_mode)
        {
            return m_u32_current_vehicle_speed_mph / 1000;
        }
        else if (OUTPUT_MODE_REPLAY == m_en_current_speed_output_mode)
        {
            return m_o_replay_curve.get_current_data();
        }
        return m_i32_manual_speed;
    }

    uint32_t SpeedSensorConverter::get_current_output_frequency() const
    {
        return m_u32_new_output_frequency_mHz;
    }

    uint32_t SpeedSensorConverter::get_current_vehicle_speed() const
    {
        return m_u32_current_vehicle_speed_mph;
    }

    uint32_t SpeedSensorConverter::get_input_pulses_per_kmph_in_mili_hertz() const
    {
        return m_u32_input_pulses_per_kmph_mHz;
    }
    void SpeedSensorConverter::set_input_pulses_per_kmph_in_mili_hertz(uint32_t value)
    {
        m_u32_input_pulses_per_kmph_mHz = value;
    }
    
    
    uint32_t SpeedSensorConverter::get_output_pulses_per_kmph_in_mili_hertz() const
    {
        return m_u32_output_pulses_per_kmph_mHz;
    }
    
    void SpeedSensorConverter::set_output_pulses_per_kmph_in_mili_hertz(uint32_t value)
    {
        m_u32_output_pulses_per_kmph_mHz = value;
    }
    
    void SpeedSensorConverter::pwm_input_capture_callback(uint32_t u32_read_frequency_in_mHz, uint32_t u32_duty_cycle)
    {
        const uint32_t u32_vehicle_speed = (u32_read_frequency_in_mHz * 1000) / m_u32_input_pulses_per_kmph_mHz;
        if(false == check_if_speed_is_valid(u32_vehicle_speed / 1000))
        {
            return;
        }
        uint32_t u32_duty_cycle_threshold = 100; // standard range accept 40%-60% duty cycle

        if (u32_vehicle_speed < 20000) // at low speeds, the frequency is so low that the duty cycle has an impact.
        {
            u32_duty_cycle_threshold = 250; //allow 25 - 75% duty cycle
        }

        // only accept values with a valid duty cycle
        if (u32_duty_cycle > 500 - u32_duty_cycle_threshold && u32_duty_cycle < 500 + u32_duty_cycle_threshold)
        {
            SpeedSensorMeasurement new_measurement;
            new_measurement.u32_frequency = u32_read_frequency_in_mHz;
            new_measurement.u32_duty_cycle = u32_duty_cycle;
            new_measurement.o_timestamp = std::chrono::system_clock::now();


            m_ast_measured_frequencies[m_u8_input_array_position++] = new_measurement;
            m_u8_input_array_position %= SPEED_SENSOR_READINGS_BUFFER_LENGTH;

            m_u32_num_of_pwm_captures++;
#if 0 /* We are in an interrupt context here, do not print out anything */
            if (u32_vehicle_speed < 6000)
            {
                // avoid too much logs - only print for speeds < 3km/h
                TRACE_LOG("SPD", LOGLEVEL_DEBUG, "IC cb %u m/h\n\r", (new_measurement.u32_frequency * 1000) / m_u32_input_pulses_per_kmph_mHz);
            }
#endif
        }
#if 0 /* We are in an interrupt context here, do not print out anything */
        else
        {
            TRACE_LOG("SPD", LOGLEVEL_ERROR, "IC cb %u m/h, incorrect duty cycle: %u\n\r",
                    u32_read_frequency_in_mHz,
                    static_cast<unsigned int>((u32_read_frequency_in_mHz * 1000) / m_u32_input_pulses_per_kmph_mHz),
                    u32_duty_cycle);

        }
#endif
    }

    void SpeedSensorConverter::cycle()
    {
        using namespace std::chrono;

        if (nullptr == m_p_output_pwm)
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_SPEED_SENSOR_CONVERTER, EXCP_TYPE_NULLPOINTER, false, __FILE__, __LINE__, 0u);
            return;
        }

        if (0u != m_u32_input_pulses_per_kmph_mHz)
        {
            // first, copy all the frequencies and convert them to m/h
            uint32_t u32_num_of_pwm_captures = 0u;
            uint32_t u32_num_of_new_readings = 0u;

            // unit is m/h
            std::vector<uint32_t> au32_measured_vehicle_speeds;

            u32_num_of_pwm_captures = m_u32_num_of_pwm_captures;

            /*
            first check if we have received any new readings at all, but maximize to not take more
            than what fits into the read buffer.
            */
            u32_num_of_new_readings = std::min<uint32_t>(
                    SPEED_SENSOR_READINGS_BUFFER_LENGTH,
                    u32_num_of_pwm_captures - m_u32_num_of_processed_pwm_captures);
            m_u32_num_of_processed_pwm_captures = u32_num_of_pwm_captures;

            auto convert_to_vehicle_speed = [&] (uint32_t u32_frequency) -> uint32_t
            {
                return (u32_frequency * 1000) / m_u32_input_pulses_per_kmph_mHz;
            };
                
#if 0
                // if not enough readings are available, capture instead all readings that still have a valid timestamp
                if (u32_num_of_new_readings > 2)
                {
                    // calculate the array index position from where to read the first value
                    u8_start_position = (m_u8_input_array_position + SPEED_SENSOR_READINGS_BUFFER_LENGTH - u32_num_of_new_readings) % SPEED_SENSOR_READINGS_BUFFER_LENGTH;
                    for (uint8_t u8_i = 0; u8_i < u32_num_of_new_readings; u8_i++)
                    {
                        // read the value and copy to the new list. New list starts with index 0
                        au32_measured_vehicle_speeds.push_back(
                                convert_to_vehicle_speed(m_ast_measured_frequencies[(u8_start_position + u8_i) % SPEED_SENSOR_READINGS_BUFFER_LENGTH].u32_frequency));
                    }
                }
                else
                {
#endif 
            // take all readings into account from the recent few timestamps
            const auto current_timestamp = system_clock::now();

            std::vector<SpeedSensorMeasurement> o_input_data(m_ast_measured_frequencies, m_ast_measured_frequencies + SPEED_SENSOR_READINGS_BUFFER_LENGTH);
            
            // remove all data which are anyway too old
            o_input_data.erase(std::remove_if(o_input_data.begin(), o_input_data.end(), [&] (const SpeedSensorMeasurement& data)
            {
                return (duration_cast<milliseconds>(current_timestamp - data.o_timestamp) > m_maximum_reading_validity);
            }), o_input_data.end());

            // then sort by their timestamp
            std::sort(o_input_data.begin(), o_input_data.end(), [](const SpeedSensorMeasurement& a, const SpeedSensorMeasurement& b)
            {
                return a.o_timestamp > b.o_timestamp;
            });
            

            // filter the remaining data based on the most recent data
            if (o_input_data.size() > 0)
            {
                /* calculate the expected vehicle speed. Depending on the vehicle speed, the number of data
                 *  taken into account is limited */
                const uint32_t cu32_measured_vehicle_speed = convert_to_vehicle_speed(o_input_data.begin()->u32_frequency);

                /* depending on the vehicle speed, the number of recent readings needed is calculated. Roughly one reading
                 per 5km/h speed is expected */
                const uint32_t u32_num_of_readings_to_take_into_account = (cu32_measured_vehicle_speed / 5000) + 1;
                //u32_num_of_readings_to_take_into_account = std::min(u32_num_of_readings_to_take_into_account, static_cast<uint32_t>(o_input_data.size()));

                /* based on the most recent data element, go back up to x periods to search for new data */
                const uint32_t cu32_search_for_data_within_period = u32_num_of_readings_to_take_into_account + 1;
                const uint32_t cu32_reading_period_ms = (1000u*1000u) / o_input_data.begin()->u32_frequency;
                milliseconds o_maximum_delay_in_ms (cu32_search_for_data_within_period * cu32_reading_period_ms);
                o_maximum_delay_in_ms = std::min(o_maximum_delay_in_ms, m_maximum_reading_validity);

                /* filter all the data which does not lie within the recent x periods */
                o_input_data.erase(std::remove_if(o_input_data.begin(), o_input_data.end(), [&] (const SpeedSensorMeasurement& data)
                {
                    return (duration_cast<milliseconds>(current_timestamp - data.o_timestamp) > o_maximum_delay_in_ms);
                }), o_input_data.end());

                // whatever is left from the data will now be taken into account, based on the number of expected readings needed.
                for(auto itr = o_input_data.begin(); itr != o_input_data.end(); ++itr)
                {
                    const uint32_t cu32_measured_vehicle_speed = convert_to_vehicle_speed(itr->u32_frequency);
                    au32_measured_vehicle_speeds.push_back(cu32_measured_vehicle_speed);
                    
                    if (au32_measured_vehicle_speeds.size() >= u32_num_of_readings_to_take_into_account)
                    {
                        break;
                    }
                }

                TRACE_LOG("SPD", LOGLEVEL_DEBUG, "filter settings: expected speed %u m/h, num of periods: %u, search range: %u ms, expected data points: %u, actual data points: %u\n\r",
                        cu32_measured_vehicle_speed,
                        u32_num_of_readings_to_take_into_account,
                        static_cast<unsigned int>(o_maximum_delay_in_ms.count()),
                        u32_num_of_readings_to_take_into_account,
                        au32_measured_vehicle_speeds.size()
                );
            }
            else
            {
                TRACE_LOG("SPD", LOGLEVEL_DEBUG, "no data after filtering by recent timestamps!\n\r");
            }
            u32_num_of_new_readings = au32_measured_vehicle_speeds.size();

            // by default, set vehicle speed to 0.Only if the input makes sense, it will be updated.
            m_u32_current_vehicle_speed_mph = 0u;

            if (u32_num_of_new_readings > 0)
            {
                char debug_str[100] = "";
                snprintf(debug_str, 99, "%u, %u, %u, %u, %u, %u, %u, %u, %u, %u",
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[0]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[1]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[2]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[3]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[4]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[5]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[6]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[7]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[8]),
                    static_cast<unsigned int>(au32_measured_vehicle_speeds[9]));
                 TRACE_LOG("SPD", LOGLEVEL_DEBUG, "%s buffer content, %u new values read\n\r", debug_str, u32_num_of_new_readings);

                // calculate average
                uint32_t u32_avg = 0u;
                for (uint8_t u8_i = 0; u8_i < u32_num_of_new_readings; u8_i++)
                {
                    u32_avg += au32_measured_vehicle_speeds[u8_i];
                }
                u32_avg /= u32_num_of_new_readings;

                bool stats_printed = false;
                uint32_t u32_gap = 0;

                while (u32_num_of_new_readings > 3) // take at least three readings into account, if filtering is employed
                {
                    // calculate min, max, and the data point furthest away from the average
                    uint32_t u32_min = au32_measured_vehicle_speeds[0];
                    uint32_t u32_max = au32_measured_vehicle_speeds[0];
                    uint32_t u32_max_deviation = 0u;
                    uint8_t u8_max_deviation_index = 0u;

                    for (uint8_t u8_i = 0; u8_i < u32_num_of_new_readings; u8_i++)
                    {
                        u32_max = std::max(u32_max, au32_measured_vehicle_speeds[u8_i]);
                        u32_min = std::min(u32_max, au32_measured_vehicle_speeds[u8_i]);

                        // check if the current point is further away from the average than the previous ones
                        uint32_t u32_current_deviation = std::abs(static_cast<int32_t>(u32_avg) - static_cast<int32_t>(au32_measured_vehicle_speeds[u8_i]));
                        if (u32_current_deviation > u32_max_deviation)
                        {
                            u32_max_deviation = u32_current_deviation;
                            u8_max_deviation_index = u8_i;
                        }
                    }

                    if (false == stats_printed)
                    {
                        stats_printed = true;
                         TRACE_LOG("SPD", LOGLEVEL_DEBUG, "Avg: %u, Min: %u, Max: %u\n\r", u32_avg, u32_min, u32_max);
                    }


                    u32_gap = u32_max - u32_min;
                    // check if the average value satisfies the termination criteria.
                    if (u32_gap < 1000)
                    {
                        // no more filtering needed
                        break;
                    }

                    // remove the item with the largest deviation from the list...
                    u32_num_of_new_readings--;
                    std::swap(au32_measured_vehicle_speeds[u32_num_of_new_readings], au32_measured_vehicle_speeds[u8_max_deviation_index]);

                    // ...and recalculate average
                    u32_avg = 0;
                    for (uint8_t u8_i = 0; u8_i < u32_num_of_new_readings; u8_i++)
                    {
                        u32_avg += au32_measured_vehicle_speeds[u8_i];
                    }
                    u32_avg /= u32_num_of_new_readings;
                }

                // in case the gap would be extremely huge, we probably have a completely wrong reading.
                if (u32_gap > 5000) // 5 km/h delta within 100ms
                {
                     TRACE_LOG("SPD", LOGLEVEL_ERROR, "Unexplicable high gap in the data, data does not make any sense! Gap is %u\n\r", u32_gap);
                    m_u32_current_vehicle_speed_mph = 0;
                }
                else
                {
                    /* the new value makes sense. Nevertheless, let's only update the value if it differs
                    by more than a minimum difference to avoid restarting the PWM. */
                    if (m_u32_current_vehicle_speed_mph < 3000 /* always update for low speeds */
                        || (std::abs(static_cast<int32_t>(m_u32_current_vehicle_speed_mph) - static_cast<int32_t>(u32_avg)) > SPEED_SENSOR_UPDATE_THRESHOLD))
                    {
                        m_u32_current_vehicle_speed_mph = u32_avg;
                    }
                }
            }
            else
            {
                clear_measured_data();
                TRACE_LOG("SPD", LOGLEVEL_DEBUG, "No new reading received, switching back to 0\n\r");
            }
        }

        // post filter vehicle speed
        
        // limit very small readings
        if (m_u32_current_vehicle_speed_mph < 1000)
        {
            m_u32_current_vehicle_speed_mph = 0;
        }
        
        // process the PWM IC capture state machine, to adapt the PWM IC prescaler to to the perfect one for this vehicle speed
        process_pwm_ic_state_machine();

        // convert to the output pulses
        int32_t i32_set_speed = m_u32_current_vehicle_speed_mph / 1000;
        if (OUTPUT_MODE_MANUAL == m_en_current_speed_output_mode)
        {
            // in manual mode, use the speed value passed from the console
            i32_set_speed = m_i32_manual_speed;
        }
        else if (OUTPUT_MODE_REPLAY == m_en_current_speed_output_mode)
        {
            m_o_replay_curve.cycle();
            i32_set_speed = m_o_replay_curve.get_current_data();
        }

        if (false == check_if_speed_is_valid(i32_set_speed))
        {
            // Somehow we converted an invalid speed value
            ExceptionHandler_handle_exception(EXCP_MODULE_SPEED_SENSOR_CONVERTER,
                    EXCP_TYPE_SPEED_SENSOR_CONVERTER_VALID_SPEED_RANGE_EXCEEEDED,
                    false, __FILE__, __LINE__, static_cast<uint32_t>(i32_set_speed));
            return;
        }
        else
        {
            m_u32_new_output_frequency_mHz = i32_set_speed * m_u32_output_pulses_per_kmph_mHz;
            uint16_t duty_cycle = 500u; // 50%

            if (SPEED_SENSOR_9000RPM == m_en_speed_sensor_variant)
            {
                /* for the 9k rpm cluster, the duty cycle is not 50/50, but is depending
                 * on the frequency. The low duration is basically always 0.5ms.
                 * Resulting unit should be ms, that's why *1000, and the frequency is
                 * given in milihertz, therefore another 1000.
                 */
                const uint32_t total_duration_in_ms = 1000 * 1000 /  m_u32_new_output_frequency_mHz;
                if (0 != total_duration_in_ms)
                {
                    duty_cycle = 500 / total_duration_in_ms; // 0.5ms

                    if (duty_cycle >= 1000)
                    {
                        ExceptionHandler_handle_exception(EXCP_MODULE_SPEED_SENSOR_CONVERTER,
                                EXCP_TYPE_SPEED_SENSOR_CONVERTER_VALID_SPEED_RANGE_EXCEEEDED,
                                false, __FILE__, __LINE__, static_cast<uint32_t>(duty_cycle));
                        duty_cycle = 100u;
                    }
                }
            }

            m_p_output_pwm->set_duty_cycle(duty_cycle);

            
            // change the pwm frequency on the output side
            m_p_output_pwm->set_frequency(m_u32_new_output_frequency_mHz);
        }
    }

    void SpeedSensorConverter::clear_measured_data()
    {
        for (uint32_t u32_i = 0u; u32_i != SPEED_SENSOR_READINGS_BUFFER_LENGTH; u32_i++)
        {
            m_ast_measured_frequencies[u32_i].u32_frequency = 0;
            m_ast_measured_frequencies[u32_i].o_timestamp = std::chrono::time_point<std::chrono::system_clock>::min();
        }
    }
    
    void SpeedSensorConverter::process_pwm_ic_state_machine()
    {
        auto activate_new_state = [&] (SpeedInputCaptureMode new_state)
        {
            // change state machine to new state
            m_en_pwm_ic_state_machine_state = new_state;
            if (nullptr != m_p_output_pwm_input_capture)
            {
                // also update the PWM IC to update to the new frequency.
                int32_t i32_ret_val = m_p_output_pwm_input_capture->set_prescaler(SpeedSensorConverterHelper::pwm_ic_state_machine[new_state].u16_pwm_ic_prescaler);
                if (i32_ret_val != OSServices::ERROR_CODE_SUCCESS)
                {
                    TRACE_LOG("SPD", LOGLEVEL_ERROR, "Setting PWM IC prescaler failed! %i\n\r", static_cast<int>(i32_ret_val));
                }
            }
            TRACE_LOG("SPD", LOGLEVEL_INFO, "Switched to PWM IC capture mode %u\n\r", static_cast<unsigned int>(new_state));
        };

        const auto current_state = SpeedSensorConverterHelper::pwm_ic_state_machine[m_en_pwm_ic_state_machine_state];
        const uint32_t cu32_current_vehicle_speed = get_current_vehicle_speed();

        if (current_state.u32_vehicle_speed_mph_lower_threshold != 0u)
        {
            if (cu32_current_vehicle_speed < current_state.u32_vehicle_speed_mph_lower_threshold)
            {
                // change to the lower setting.
                activate_new_state(current_state.en_lower_capture_mode);
            }
        }
        if (current_state.u32_vehicle_speed_mph_upper_threshold != 0u)
        {
            if (cu32_current_vehicle_speed > current_state.u32_vehicle_speed_mph_upper_threshold)
            {
                // change to the higher setting.
                activate_new_state(current_state.en_higher_capture_mode);
            }
        }
    }

    bool SpeedSensorConverter::check_if_speed_is_valid(int32_t i32_speed_value_in_kmph)
    {
        if (i32_speed_value_in_kmph > 250 || i32_speed_value_in_kmph < -50)
        {
            return false;
        }
        return true;
    }
} /* namespace app */
