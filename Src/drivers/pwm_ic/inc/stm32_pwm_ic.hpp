#ifndef _STM32_PWM_IC_HPP_
#define _STM32_PWM_IC_HPP_

#include "generic_pwm_ic.hpp"
#include "hwheader.h"

namespace drivers
{
    class STM32PWM_IC : public GenericPWM_IC
    {
    public:

        STM32PWM_IC(TIM_TypeDef* pt_timer_unit, uint32_t u32_first_channel, uint32_t u32_second_channel, uint32_t u32_prescaler, uint16_t u16_arr);

        virtual ~STM32PWM_IC() {}

        virtual int32_t init();

        virtual int32_t deinit();

        /** Reads the duty cycle and the frequency of the set port */
        virtual int32_t read_frequency_and_duty_cycle(uint32_t &u32_frequency_in_milihz, uint32_t &u32_duty_cycle_permil);

        /** configure a callback, if desired */
        virtual int32_t set_capture_callback(void(*p_callback)(GenericPWM_IC*, uint32_t, uint32_t));

        void process_capture_callback(TIM_HandleTypeDef *htim);
    private:
        void Error_Handler(void);

        TIM_TypeDef* m_pt_timer_unit;
        const uint32_t m_u32_first_channel;
        const uint32_t m_u32_second_channel;
        const uint32_t m_u32_prescaler;
        const uint16_t m_u16_arr;

        TIM_HandleTypeDef m_timer_handle;

        uint32_t m_u32_frequency_in_milihz;
        uint32_t m_u32_duty_cycle_permil;

        bool m_bo_initialized;

        uint32_t m_o_last_input_capture_timestamp;

        /** Callback to be executed when the capture callback is triggered */
        void(*m_p_callback)(GenericPWM_IC*, uint32_t, uint32_t);
    };
}


#endif /* _STM32_PWM_IC_HPP_ */
