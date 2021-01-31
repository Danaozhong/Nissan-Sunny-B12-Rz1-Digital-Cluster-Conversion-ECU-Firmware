#ifndef _PWM_TESTER_HPP_
#define _PWM_TESTER_HPP_

#include "speed_sensor_converter.hpp"
#include "os_console.hpp"
#include <chrono>

namespace app
{
    struct PWMTestSteps
    {
        uint32_t u32_input_speed_mph;
        std::chrono::milliseconds o_calibration_time;
        uint32_t u32_maximum_error_mph;
    };
    
    class CommandPWMTester : public OSServices::Command
    {
    public:
        CommandPWMTester() : Command("test_pwm") {}

        virtual ~CommandPWMTester() {}

        virtual int32_t command_main(const char** params, uint32_t u32_num_of_params, OSServices::OSConsoleGenericIOInterface& p_o_io_interface);
        
        void set_speed_sensor_converter(SpeedSensorConverter* p_converter);
    private:
       std::vector<PWMTestSteps> m_ao_test_steps;

       bool m_bo_program_running;
       SpeedSensorConverter* m_po_speed_sensor_converter_under_test;
    };

}

#endif 