#include "pwm_tester.hpp"

namespace app
{
    int32_t CommandPWMTester::command_main(const char** params, uint32_t u32_num_of_params,
            OSServices::OSConsoleGenericIOInterface& p_o_io_interface)

    {
        if (nullptr == m_po_speed_sensor_converter_under_test)
        {
            return OSServices::ERROR_CODE_PARAMETER_WRONG;
        }
        
        // create the test data
        m_ao_test_steps = {
            {0, std::chrono::milliseconds(500), 0},
            {5*1000, std::chrono::milliseconds(600), 2500},
            {5*1000, std::chrono::milliseconds(300), 500},
            {7*1000, std::chrono::milliseconds(600), 2000},
            {7*1000, std::chrono::milliseconds(300), 500},
            {10*1000, std::chrono::milliseconds(400), 1000},
            {10*1000, std::chrono::milliseconds(200), 500},
            {12*1000, std::chrono::milliseconds(500), 1000},
            {12*1000, std::chrono::milliseconds(200), 500},
            {15*1000, std::chrono::milliseconds(500), 1000},
            {15*1000, std::chrono::milliseconds(200), 500},  
            {11*1000, std::chrono::milliseconds(400), 1000},
            {11*1000, std::chrono::milliseconds(300), 500},
            {3*1000, std::chrono::milliseconds(600), 2000},
            {3*1000, std::chrono::milliseconds(600), 500},
            {15*1000, std::chrono::milliseconds(800), 300},
            {50*1000, std::chrono::milliseconds(300), 300},
            {100*1000, std::chrono::milliseconds(300), 500},
            {150*1000, std::chrono::milliseconds(300), 500},
            {200*1000, std::chrono::milliseconds(300), 1000},
            {200*1000, std::chrono::milliseconds(100), 600},
            {250*1000, std::chrono::milliseconds(300), 1000},
            {250*1000, std::chrono::milliseconds(100), 600},
            {3*1000, std::chrono::milliseconds(700), 3000},
            {3*1000, std::chrono::milliseconds(700), 500},
            {0*1000, std::chrono::milliseconds(700), 500},
            {4*1000, std::chrono::milliseconds(600), 300},
            {100*1000, std::chrono::milliseconds(400), 500},
            {20*1000, std::chrono::milliseconds(600), 500},
            {0*1000, std::chrono::milliseconds(700), 500},
            {200*1000, std::chrono::milliseconds(300), 1000},
            {200*1000, std::chrono::milliseconds(200), 600},
        };
        
        m_bo_program_running = true;

        //make sure frequencies are matched (input is wired to output!)
        m_po_speed_sensor_converter_under_test->set_output_pulses_per_kmph_in_mili_hertz(m_po_speed_sensor_converter_under_test->get_input_pulses_per_kmph_in_mili_hertz());
        
        // set to manual mode to be able to feed in predefined speed values
        m_po_speed_sensor_converter_under_test->set_speed_output_mode(OUTPUT_MODE_MANUAL);
        
        uint32_t u32_test_step = 0u;
        for (auto itr = m_ao_test_steps.begin(); itr != m_ao_test_steps.end(); ++itr)
        {
            u32_test_step++;
            
            m_po_speed_sensor_converter_under_test->set_manual_speed(itr->u32_input_speed_mph);
            std_ex::sleep_for(itr->o_calibration_time);
            const int32_t ci32_actual_speed = m_po_speed_sensor_converter_under_test->get_current_vehicle_speed();
            const int32_t ci32_set_speed = static_cast<int32_t>(itr->u32_input_speed_mph);
            
            const uint32_t cu32_error = static_cast<uint32_t>(std::abs(ci32_set_speed - ci32_actual_speed));
            
            printf("[%u/%u] Set speed: %u.%02u km/h, read speed: %u.%02u km/h, error: %u, test result: ",
                static_cast<unsigned int>(u32_test_step), static_cast<unsigned int>(m_ao_test_steps.size()),
                static_cast<unsigned int>(ci32_set_speed / 1000), static_cast<unsigned int>(ci32_set_speed % 1000),
                static_cast<unsigned int>(ci32_actual_speed / 1000), static_cast<unsigned int>(ci32_actual_speed % 1000),
                static_cast<unsigned int>(cu32_error));

            if (cu32_error > itr->u32_maximum_error_mph)
            {
                printf("[FAILED]\n\r");
            }
            else
            {
                printf("[SUCCESS]\n\r");
            }
        }
        
        return OSServices::ERROR_CODE_SUCCESS;
    }
    
    void CommandPWMTester::set_speed_sensor_converter(SpeedSensorConverter* p_converter)
    {
        m_po_speed_sensor_converter_under_test = p_converter;
    }
}

