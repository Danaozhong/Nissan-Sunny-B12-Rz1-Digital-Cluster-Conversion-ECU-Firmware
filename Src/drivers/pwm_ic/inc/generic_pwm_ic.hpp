#ifndef _GENERIC_PWM_IC_HPP_
#define _GENERIC_PWM_IC_HPP_

#include <cstdint>

namespace drivers
{
    /** Generic Driver interface to access a PWM Input Capture (read PWM duty cycle and frequency) */
    class GenericPWM_IC
    {
    public:

        virtual ~GenericPWM_IC() {}

        virtual int32_t init() = 0;

        virtual int32_t deinit() = 0;

        /** Reads the duty cycle and the frequency of the set port */
        virtual int32_t read_frequency_and_duty_cycle(uint32_t &u32_frequency_in_milihz, uint32_t &u32_duty_cylce_permil) = 0;

        /** Set a function callback */
        virtual int32_t set_capture_callback(void(*p_callback)(GenericPWM_IC*, uint32_t, uint32_t)) = 0;
    };
}


#endif /* _GENERIC_PWM_IC_HPP_ */
