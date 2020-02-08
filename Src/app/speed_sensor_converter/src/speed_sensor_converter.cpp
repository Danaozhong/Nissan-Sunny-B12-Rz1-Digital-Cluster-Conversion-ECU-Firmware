#include "speed_sensor_converter.hpp"
#include "os_console.hpp"
#include "excp_handler_if.h"

namespace app
{
    SpeedSensorConverter::SpeedSensorConverter(std::shared_ptr<drivers::GenericPWM> p_output_pwm,
                std::shared_ptr<drivers::GenericPWM_IC> p_output_pwm_input_capture,
                uint32_t u32_input_pulses_per_kmph_mHz,
                uint32_t u32_output_pulses_per_kmph_mHz)
    : m_p_output_pwm(p_output_pwm), m_p_output_pwm_input_capture(p_output_pwm_input_capture),
      m_en_current_speed_output_mode(OUTPUT_MODE_CONVERSION),
      m_i32_manual_speed(0), m_u32_current_vehicle_speed_kmph(0u),
      m_u32_input_pulses_per_kmph_mHz(u32_input_pulses_per_kmph_mHz),
      m_u32_output_pulses_per_kmph_mHz(u32_output_pulses_per_kmph_mHz)
    {
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
    }

    SpeedSensorConverter::~SpeedSensorConverter()
    {
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

    int32_t SpeedSensorConverter::get_current_input_speed() const
    {
        return m_u32_current_vehicle_speed_kmph;
    }

    int32_t SpeedSensorConverter::get_current_speed() const
    {
        if (OUTPUT_MODE_CONVERSION == m_en_current_speed_output_mode)
        {
            return m_u32_current_vehicle_speed_kmph;
        }
        else if (OUTPUT_MODE_REPLAY == m_en_current_speed_output_mode)
        {
            return m_o_replay_curve.get_current_data();
        }
        return m_i32_manual_speed;
    }

    uint32_t SpeedSensorConverter::get_current_frequency() const
    {
        return m_u32_new_output_frequency_mHz;
    }

    uint32_t SpeedSensorConverter::get_current_input_frequency() const
    {
        return m_u32_input_frequency_mHz;
    }

    void SpeedSensorConverter::cycle()
    {
        if (nullptr == m_p_output_pwm_input_capture || nullptr == m_p_output_pwm)
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_SPEED_SENSOR_CONVERTER, EXCP_TYPE_NULLPOINTER, false, __FILE__, __LINE__, 0u);
            return;
        }

        uint32_t input_duty_cylce = 0u;
        m_u32_input_frequency_mHz = 0u;
        // read the current frequency from the speed sensor
        if (OSServices::ERROR_CODE_SUCCESS != m_p_output_pwm_input_capture->read_frequency_and_duty_cycle(m_u32_input_frequency_mHz, input_duty_cylce))
        {
            ExceptionHandler_handle_exception(EXCP_MODULE_SPEED_SENSOR_CONVERTER, EXCP_TYPE_SPEED_SENSOR_CONVERTER_PWM_READ_FAILED, false, __FILE__, __LINE__, 0u);
            return;
        }

        // convert to the output pulses
        if (0u != m_u32_input_pulses_per_kmph_mHz)
        {
            m_u32_current_vehicle_speed_kmph = m_u32_input_frequency_mHz / m_u32_input_pulses_per_kmph_mHz;
        }

        int32_t i32_set_speed = m_u32_current_vehicle_speed_kmph;
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
