#include "trace_if.h"
#include "stm32_pwm.hpp"

#include <cassert>

namespace drivers
{
    STM32PWM::STM32PWM(TIM_TypeDef* pt_timer_unit, uint32_t u32_timer_channel,
            GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin)
    : m_u32_timer_channel(u32_timer_channel),
      m_u32_configured_frequency_millihertz(123u), // set a random initial value, is set to 0 later
      m_u16_configured_duty_cycle(500u), // set to 50% initially
      m_pt_gpio_block(pt_gpio_block),
      m_u16_gpio_pin(u16_gpio_pin)
    {
        
        TRACE_DECLARE_CONTEXT("PWM");
        
        /* Do the port configuration */
        
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* TIMx Peripheral clock enable */
        if (TIM1 == pt_timer_unit)
        {
            __HAL_RCC_TIM1_CLK_ENABLE();
        }
        else if (TIM2 == pt_timer_unit)
        {
            __HAL_RCC_TIM2_CLK_ENABLE();
        }
        else if (TIM3 == pt_timer_unit)
        {
            __HAL_RCC_TIM3_CLK_ENABLE();
        }

        /*  enable all GPIO Channels Clock requested */
        if (GPIOA == pt_gpio_block)
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        }
        else if (GPIOB == pt_gpio_block)
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        }
        else if (GPIOC == pt_gpio_block)
        {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        }

#ifdef HAL_TIM_MODULE_ENABLED
         o_timer_handle.Instance = pt_timer_unit;
#endif
         // set a frequency of 0
         set_frequency(0);
    }

    int32_t STM32PWM::configure_gpio_as_pwm()
    {
        GPIO_InitTypeDef   GPIO_InitStruct;

        /* Common configuration for all channels */
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pin = m_u16_gpio_pin;

        if (TIM1 == o_timer_handle.Instance)
        {
            GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
        }
        else if (TIM2 == o_timer_handle.Instance)
        {
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        }
        else if (TIM3 == o_timer_handle.Instance)
        {
            GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        }
        else
        {
            TRACE_LOG("TIM", LOGLEVEL_ERROR, "The selected timer unit is not supported!");
            assert(false);
            // implement other cases, when needed
        }

        // unconfigure previous config, and reconfigure the pin as PWM
        HAL_GPIO_DeInit(m_pt_gpio_block, m_u16_gpio_pin);
        HAL_GPIO_Init(m_pt_gpio_block, &GPIO_InitStruct);
        return 0;
    }

    int32_t STM32PWM::configure_gpio_as_high()
    {
        HAL_GPIO_DeInit(m_pt_gpio_block, m_u16_gpio_pin);

        /* reconfigure the pin to output / push pull */
        GPIO_InitTypeDef   GPIO_InitStruct;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pin = m_u16_gpio_pin;
        HAL_GPIO_Init(m_pt_gpio_block, &GPIO_InitStruct);

        // set GPIO to low/high
#if 0
        // use this if you want the GPIO pin to be high on frequency 0
        HAL_GPIO_WritePin(m_pt_gpio_block, m_u16_gpio_pin, GPIO_PIN_SET);
#endif
        HAL_GPIO_WritePin(m_pt_gpio_block, m_u16_gpio_pin, GPIO_PIN_RESET);
        return 0;
    }

    int32_t STM32PWM::reconfigure_pwm(uint32_t u32_frequency, uint16_t u16_duty_cycle)
    {
        if (0 == u32_frequency || u16_duty_cycle > 1000u)
        {
            return -1;
        }

        /* Compute the prescaler value to have TIM3 counter clock equal to 24000000 Hz */
        // 3.5 Hz
        //uhPrescalerValue = (uint32_t)(SystemCoreClock / 2400) - 1; // 2km/h 3,6Hz
        //uhPrescalerValue = (uint32_t)(SystemCoreClock / 24000) - 1; // 10km/h 36,1 Hz
        //uhPrescalerValue = (uint32_t)(SystemCoreClock / 240000) - 1; // 86km/h -- 360,9Hz
        //uhPrescalerValue = (uint32_t)(SystemCoreClock / 480000) - 1; // 171km/h  -- 721Hz
        //uhPrescalerValue = (uint32_t)(SystemCoreClock / 960000) - 1; // 180km/h/h  -- 1444kHz
        const uint32_t u32_timer_clock = 24000;

        u32_prescaler_value = static_cast<uint32_t>(SystemCoreClock / u32_timer_clock) - 1;

        // timer is currently clocked at 24kHz
        // f = timer_clock / u32_timer_value
        // u32_timer_value = 24kHz / f
        u32_timer_value = u32_timer_clock * 1000 / u32_frequency - 1;

        /*##-1- Configure the TIM peripheral #######################################*/
        /* -----------------------------------------------------------------------
        TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles.

        In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1),
        since APB1 prescaler is different from 1.
        TIM3CLK = 2 * PCLK1
        PCLK1 = HCLK / 2
        => TIM3CLK = HCLK = SystemCoreClock

        To get TIM3 counter clock at 24 MHz, the prescaler is computed as follows:
        Prescaler = (TIM3CLK / TIM3 counter clock) - 1
        Prescaler = (SystemCoreClock /24 MHz) - 1

        To get TIM3 output clock at 36 KHz, the period (ARR)) is computed as follows:
        ARR = (TIM3 counter clock / TIM3 output clock) - 1
        = 665

        TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
        TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
        TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
        TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%

        Note:
        SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f3xx.c file.
        Each time the core clock (HCLK) changes, user had to update SystemCoreClock
        variable value. Otherwise, any configuration based on this variable will be incorrect.
        This variable is updated in three ways:
        1) by calling CMSIS function SystemCoreClockUpdate()
        2) by calling HAL API function HAL_RCC_GetSysClockFreq()
        3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
        ----------------------------------------------------------------------- */

        /* Initialize TIMx peripheral as follows:
        + Prescaler = (SystemCoreClock / 24000000) - 1
        + Period = (665 - 1)
        + ClockDivision = 0
        + Counter direction = Up
        */
        o_timer_handle.Init.Prescaler         = u32_prescaler_value;
        o_timer_handle.Init.Period            = u32_timer_value;
        o_timer_handle.Init.ClockDivision     = 0;
        o_timer_handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
        o_timer_handle.Init.RepetitionCounter = 0;
        o_timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        if (HAL_TIM_PWM_Init(&o_timer_handle) != HAL_OK)
        {
            /* Initialization Error */
            TRACE_LOG("PWM", LOGLEVEL_ERROR, "PWM initialization failed!");
            return -2;
        }

        /*##-2- Configure the PWM channels #########################################*/
        /* Common configuration for all channels */
        sConfig.OCMode       = TIM_OCMODE_PWM1;
        sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
        sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
        sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
        sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

        sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

        /* Set the pulse value for channel 1 */
        sConfig.Pulse = u32_timer_value * static_cast<uint32_t>(u16_duty_cycle) / 1000;
        if (HAL_TIM_PWM_ConfigChannel(&o_timer_handle, &sConfig, m_u32_timer_channel) != HAL_OK)
            {
            /* Configuration Error */
            TRACE_LOG("PWM", LOGLEVEL_ERROR, "PWM channel configuration failed!");
            return -3;
        }
        else
        {
            /*##-3- Start PWM signals generation #######################################*/
            /* Start channel 1 */
            if (HAL_TIM_PWM_Start(&o_timer_handle, m_u32_timer_channel) != HAL_OK)
            {
                /* PWM Generation Error */
                TRACE_LOG("PWM", LOGLEVEL_ERROR, "PWM start failed!");
                return -4;
            }
        }
        return 0;
    }

    STM32PWM::~STM32PWM()
    {
        HAL_TIM_PWM_Stop(&o_timer_handle, m_u32_timer_channel);  // Stop PWM
        
        if (HAL_TIM_PWM_DeInit(&o_timer_handle) != HAL_OK)
        {
            /* De-initialization Error */
            TRACE_LOG("PWM", LOGLEVEL_ERROR, "PWM deinitialization failed!");
        }
    }

    void STM32PWM::set_frequency(uint32_t u32_frequency_mhz)
    {
        if (m_u32_configured_frequency_millihertz != u32_frequency_mhz)
        {
            if (0 == m_u32_configured_frequency_millihertz)
            {
                // reconfigure GPIO from normal GPIO to PWM output
                configure_gpio_as_pwm();
            }
            HAL_TIM_PWM_Stop(&o_timer_handle, m_u32_timer_channel);  // Stop PWM
            if (0 != u32_frequency_mhz)
            {
                reconfigure_pwm(u32_frequency_mhz, m_u16_configured_duty_cycle); // re-initialize the Timer2
            }
            else
            {
                // set the pin to GPIO high (no output at the moment)
                configure_gpio_as_high();
            }
            m_u32_configured_frequency_millihertz = u32_frequency_mhz;
        }
    }

    void STM32PWM::set_duty_cycle(uint16_t u16_duty_cycle)
    {
        m_u16_configured_duty_cycle = u16_duty_cycle;
        // TODO do not update for now, this will be done at some later stage.
    }
}
