
#ifndef _GENERIC_PWM_HPP_
#define _GENERIC_PWM_HPP_

namespace drivers
{
    class GenericPWM
    {
    public:
        /** Destructor */
        virtual ~GenericPWM() {}

        /** Function to set the frequency of the PWM. Unit given in mili Hertz */
        virtual void set_frequency(uint32_t u32_frequency_mhz) = 0;

        /** Function to set the duty cycle of the PWM */
        virtual void set_duty_cycle(uint32_t u32_duty_cycle) = 0;
    };

}

#endif /* _GENERIC_PWM_HPP_ */
