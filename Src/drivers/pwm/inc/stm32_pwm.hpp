
#ifndef _STM32_PWM_HPP_
#define _STM32_PWM_HPP_

/* System headers */
#include "hwheader.h"

/* Own headers */
#include "generic_pwm.hpp"

namespace drivers
{
	class STM32PWM : public GenericPWM
	{
	public:
		STM32PWM(TIM_TypeDef* pt_timer_unit, uint32_t u32_timer_channel,
				GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin);
		virtual ~STM32PWM();

		/** Sets the frequency. Unit is milli hertz */
		virtual void set_frequency(uint32_t u32_frequency_millihertz);

		virtual void set_duty_cycle(uint32_t u32_duty_cycle);

	private:
		void Error_Handler(void);
		int32_t calculate_prescaler_value(uint32_t u32_frequency_mhz) const;

		/** Configures the given GPIO pin as a PWM output */
		int32_t configure_gpio_as_pwm();

		/** Configures the given GPIO pin as high (used for frequency 0) */
		int32_t configure_gpio_as_high();

		int32_t reconfigure_pwm(uint32_t u32_frequency, uint32_t u32_duty_cycle);

#ifdef HAL_TIM_MODULE_ENABLED
		/* Timer handler declaration */
		TIM_HandleTypeDef    o_timer_handle;
		uint32_t m_u32_timer_channel;
		/* Timer Output Compare Configuration Structure declaration */
		TIM_OC_InitTypeDef sConfig;
		/* Counter Prescaler value */
		uint32_t u32_prescaler_value;

		uint32_t u32_timer_value;

		// currently configured frequency
		uint32_t m_u32_configured_frequency_millihertz;

		GPIO_TypeDef* m_pt_gpio_block;
		uint16_t m_u16_gpio_pin;

#endif

	};

}

#endif /* _STM32_PWM_HPP_ */
