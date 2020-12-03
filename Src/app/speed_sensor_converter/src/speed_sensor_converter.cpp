#include "speed_sensor_converter.hpp"
#include "os_console.hpp"
#include "excp_handler_if.h"
#include "trace_if.h"

#include <cmath>




    app::SpeedSensorConverter* po_speed_sensor_converter_instance = nullptr;
    void speed_sensor_pwm_input_capture_callback(drivers::GenericPWM_IC* p_ic_obj, uint32_t u32_read_frequency, uint32_t u32_read_duty_cyclce)
    {
        /* TODO you could do a mapping here, but let's be simple */
        if (nullptr != po_speed_sensor_converter_instance)
        {
            po_speed_sensor_converter_instance->pwm_input_capture_callback(u32_read_frequency);
        }
    }
    namespace app
    {

    SpeedSensorConverter::SpeedSensorConverter(drivers::GenericPWM* p_output_pwm,
                drivers::GenericPWM_IC* p_output_pwm_input_capture,
                uint32_t u32_input_pulses_per_kmph_mHz,
                uint32_t u32_output_pulses_per_kmph_mHz)
    : m_p_output_pwm(p_output_pwm), m_p_output_pwm_input_capture(p_output_pwm_input_capture),
      m_en_current_speed_output_mode(OUTPUT_MODE_CONVERSION),
      m_i32_manual_speed(75), m_au32_input_frequency_mHz( { 0 } ), m_u8_input_array_position(0u),
      m_u32_current_vehicle_speed_mph(0u),
      m_u32_input_pulses_per_kmph_mHz(u32_input_pulses_per_kmph_mHz),
      m_u32_output_pulses_per_kmph_mHz(u32_output_pulses_per_kmph_mHz),
      m_u32_num_of_pwm_captures(0u), m_u32_num_of_processed_pwm_captures(0u)
    {
        static_assert(SPEED_SENSOR_READINGS_BUFFER_LENGTH > 0);

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


        app::CharacteristicCurve<int32_t, int32_t> replay_dataset(replay_curve_data, sizeof(replay_curve_data) / sizeof(replay_curve_data[0]));

        m_o_replay_curve.load_data(replay_dataset);

#ifdef SPEED_CONVERTER_USE_OWN_TASK
        m_bo_terminate_thread = false;
        // Create the thread which cyclically converts the speed sensor signals
        auto o_main_func = std::bind(&SpeedSensorConverter::speed_sensor_converter_main, this);
        m_p_data_conversion_thread = new std_ex::thread(o_main_func, "Speed_Conv", 1u, 0x800);
#endif

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
            //m_p_output_pwm_input_capture->set_capture_callback(nullptr);
        }
        //po_speed_sensor_converter_instance = nullptr;
#ifdef SPEED_CONVERTER_USE_OWN_TASK
        // notify the data thread that the object is destroyed
        m_bo_terminate_thread = true;
        if (nullptr != this->m_p_data_conversion_thread)
        {
            // wait for the thread to finish
            this->m_p_data_conversion_thread->join();
            delete this->m_p_data_conversion_thread;
        }
#endif
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

    void SpeedSensorConverter::poll_vehicle_speed()
    {
        if ( nullptr == m_p_output_pwm_input_capture)
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_SPEED_SENSOR_CONVERTER, EXCP_TYPE_NULLPOINTER, false, __FILE__, __LINE__, 0u);
            return;
        }

        {
            //std::lock_guard<std::mutex> lock(m_pwm_capture_data_mutex);

            // read the current frequency from the speed sensor
            uint32_t input_duty_cylce = 0u;

            if (OSServices::ERROR_CODE_SUCCESS != m_p_output_pwm_input_capture->read_frequency_and_duty_cycle(
                    m_au32_input_frequency_mHz[m_u8_input_array_position++], input_duty_cylce))
            {
                ExceptionHandler_handle_exception(EXCP_MODULE_SPEED_SENSOR_CONVERTER, EXCP_TYPE_SPEED_SENSOR_CONVERTER_PWM_READ_FAILED, false, __FILE__, __LINE__, 0u);
                return;
            }
            m_u32_num_of_pwm_captures++;
            m_u8_input_array_position %= SPEED_SENSOR_READINGS_BUFFER_LENGTH;
        }
    }

    void SpeedSensorConverter::pwm_input_capture_callback(int32_t i32_read_frequency_in_mHz)
    {
        {
        //std::lock_guard<std::mutex> lock(m_pwm_capture_data_mutex);
            m_au32_input_frequency_mHz[m_u8_input_array_position++] = i32_read_frequency_in_mHz;
            m_u8_input_array_position %= SPEED_SENSOR_READINGS_BUFFER_LENGTH;
            m_u32_num_of_pwm_captures++;
        }

        DEBUG_PRINTF("Measured frequency: %i, measured speed: %i m/h\n\r", i32_read_frequency_in_mHz, (i32_read_frequency_in_mHz * 1000) / m_u32_input_pulses_per_kmph_mHz);
    }

    void SpeedSensorConverter::cycle()
    {
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
            uint32_t au32_measured_vehicle_speeds[SPEED_SENSOR_READINGS_BUFFER_LENGTH];
            uint8_t u8_start_position = 0u;
            {
                //std::lock_guard<std::mutex> lock(m_pwm_capture_data_mutex);
                u32_num_of_pwm_captures = m_u32_num_of_pwm_captures;

                /*
                first check if we have received any new readings at all, but maximize to not take more
                than what fits into the read buffer.
                */
                u32_num_of_new_readings = std::min<uint32_t>(
                        SPEED_SENSOR_READINGS_BUFFER_LENGTH,
                        u32_num_of_pwm_captures - m_u32_num_of_processed_pwm_captures);
                m_u32_num_of_processed_pwm_captures = u32_num_of_pwm_captures;

                // calculate the array index position from where to read the first value
                u8_start_position = (m_u8_input_array_position + SPEED_SENSOR_READINGS_BUFFER_LENGTH - u32_num_of_new_readings) % SPEED_SENSOR_READINGS_BUFFER_LENGTH;
                for (uint8_t u8_i = 0; u8_i < u32_num_of_new_readings; u8_i++)
                {
                    // read the value and copy to the new list. New list starts with index 0
                    au32_measured_vehicle_speeds[u8_i] =
                            (m_au32_input_frequency_mHz[(u8_start_position + u8_i) % SPEED_SENSOR_READINGS_BUFFER_LENGTH] * 1000)
                            / m_u32_input_pulses_per_kmph_mHz;
                }
            }

            // mutex lock is released now, now we can spend more effort on doing the actual calculations.

            // by default, set vehicle speed to 0.Only if the input makes sense, it will be updated.
            m_u32_current_vehicle_speed_mph = 0u;

            if (u32_num_of_new_readings > 0)
            {
                char debug_str[100] = "";
                snprintf(debug_str, 99, "%u, %u, %u, %u, %u, %u, %u, %u, %u, %u",
                        au32_measured_vehicle_speeds[0],
                        au32_measured_vehicle_speeds[1],
                        au32_measured_vehicle_speeds[2],
                        au32_measured_vehicle_speeds[3],
                        au32_measured_vehicle_speeds[4],
                        au32_measured_vehicle_speeds[5],
                        au32_measured_vehicle_speeds[6],
                        au32_measured_vehicle_speeds[7],
                        au32_measured_vehicle_speeds[8],
                        au32_measured_vehicle_speeds[9]);
                DEBUG_PRINTF("%s buffer content, %u new values read\n\r", debug_str, u32_num_of_new_readings);

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
                        DEBUG_PRINTF("Avg: %u, Min: %u, Max: %u\n\r", u32_avg, u32_min, u32_max);
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
                if (u32_gap > 10000) // 10 km/h delta within 10ms
                {
                    DEBUG_PRINTF("Unexplicable high gap in the data, data does not make any sense! Gap is %u\n\r", u32_gap);
                    m_u32_current_vehicle_speed_mph = 0;
                }
                else
                {
                    // display the filtered value.
                    m_u32_current_vehicle_speed_mph = u32_avg;
                }
            }
        }

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
            // change the pwm frequency on the output side
            m_p_output_pwm->set_frequency(m_u32_new_output_frequency_mHz);
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

#ifdef SPEED_CONVERTER_USE_OWN_TASK
    void SpeedSensorConverter::speed_sensor_converter_main()
    {
        while(false == m_bo_terminate_thread)
        {
            this->cycle();

            // suspend the task fro 250ms
            std_ex::sleep_for(std::chrono::milliseconds(250));
        }
    }
#endif
}
