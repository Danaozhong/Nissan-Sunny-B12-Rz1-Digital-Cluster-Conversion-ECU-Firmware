#include "speed_sensor_converter.hpp"
#include "os_console.hpp"

namespace app
{
    SpeedSensorConverter::SpeedSensorConverter(std::shared_ptr<drivers::GenericPWM> p_output_pwm,
                std::shared_ptr<drivers::GenericPWM_IC> p_output_pwm_input_capture,
                uint32_t u32_input_pulses_per_kmph_mHz,
                uint32_t u32_output_pulses_per_kmph_mHz)
    : m_p_output_pwm(p_output_pwm), m_p_output_pwm_input_capture(p_output_pwm_input_capture),
      m_p_data_conversion_thread(nullptr),
      m_en_current_speed_output_mode(OUTPUT_MODE_CONVERSION),
      m_i32_manual_speed(0), m_u32_current_vehicle_speed_kmph(0u),
      m_u32_input_pulses_per_kmph_mHz(u32_input_pulses_per_kmph_mHz),
      m_u32_output_pulses_per_kmph_mHz(u32_output_pulses_per_kmph_mHz),
      m_bo_terminate_thread(false)
    {
        // Create the thread which cyclically converts the speed sensor signals
        auto o_main_func = std::bind(&SpeedSensorConverter::speed_sensor_converter_main, this);
        m_p_data_conversion_thread = new std_ex::thread(o_main_func, "Speed_Conv", 1u, 0x1000);
    }

    SpeedSensorConverter::~SpeedSensorConverter()
    {
        // notify the data thread that the object is destroyed
        m_bo_terminate_thread = true;
        if (nullptr != this->m_p_data_conversion_thread)
        {
            // wait for the thread to finish
            this->m_p_data_conversion_thread->join();
            delete this->m_p_data_conversion_thread;
        }

        // everything else is cleaned up automatically
    }

    /** Use this to select the mode in which the speed signal is sent out to the cluster.
     * use OUTPUT_MODE_CONVERSION to derive the speed signal from the input speed sensor,
     * alternatively; use OUTPUT_MODE_MANUAL to manually configure a speed value. */
    void SpeedSensorConverter::set_speed_output_mode(SpeedOutputMode en_speed_output_mode)
    {
        this->m_en_current_speed_output_mode = en_speed_output_mode;
    }

    /** When the speed sensor conversion is in manual mode, use this function to set the manual
     * speed value.
     * \param[in] i32_speed_in_mph  The velocity in meters / hour.
     */
    int32_t SpeedSensorConverter::set_manual_speed(int32_t i32_speed_in_mph)
    {
        // speed limit protection, make sure to not damage the cluster
        if (i32_speed_in_mph < 250000 && i32_speed_in_mph > -250000)
        {
            this->m_i32_manual_speed = i32_speed_in_mph;
            return 0;
        }
        return -1;
    }

    void SpeedSensorConverter::cycle()
    {
        if (nullptr == m_p_output_pwm_input_capture || nullptr == m_p_output_pwm)
        {
            return;
        }

        uint32_t input_duty_cylce = 0u;
        uint32_t input_frequency_mHz = 0u;
        // read the current frequency from the speed sensor
        if (OSServices::ERROR_CODE_SUCCESS != m_p_output_pwm_input_capture->read_frequency_and_duty_cycle(input_frequency_mHz, input_duty_cylce))
        {
            return;
        }

        // convert to the output pulses
        if (0u != m_u32_input_pulses_per_kmph_mHz)
        {
            m_u32_current_vehicle_speed_kmph = input_frequency_mHz / m_u32_input_pulses_per_kmph_mHz;
        }

        int32_t new_output_frequency_mHz;
        if (OUTPUT_MODE_CONVERSION == m_en_current_speed_output_mode)
        {
            // in this mode, derive the output speed from the signal of the speed sensor
            new_output_frequency_mHz = m_u32_current_vehicle_speed_kmph * m_u32_output_pulses_per_kmph_mHz;
        }
        else
        {
            // in manual mode, use the speed value passed from the console
            new_output_frequency_mHz = m_i32_manual_speed * m_u32_output_pulses_per_kmph_mHz;
        }

        // change the pwm frequency on the output side
        m_p_output_pwm->set_frequency(new_output_frequency_mHz);
    }

    void SpeedSensorConverter::speed_sensor_converter_main()
    {
        while(false == m_bo_terminate_thread)
        {
            this->cycle();

            // suspend the task fro 250ms
            std_ex::sleep_for(std::chrono::milliseconds(250));
        }

    }
}
