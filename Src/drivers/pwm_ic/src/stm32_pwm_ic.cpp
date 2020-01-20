#include "stm32_pwm_ic.hpp"
#include "os_console.hpp"

#include <map>

namespace
{
    std::map<const TIM_HandleTypeDef*, drivers::STM32PWM_IC*> map_tim_handles_to_object;
}
namespace drivers
{

    //STM32PWM_IC::STM32PWM_IC(TIM_TypeDef* pt_timer_unit, uint32_t u32_prescaler, uint8_t u16_arr)
    STM32PWM_IC::STM32PWM_IC(TIM_TypeDef* pt_timer_unit, uint32_t u32_first_channel, uint32_t u32_second_channel, uint32_t u32_prescaler, uint16_t u16_arr)
        : m_pt_timer_unit(pt_timer_unit),
          m_u32_first_channel(u32_first_channel),
          m_u32_second_channel(u32_second_channel),
          m_u32_prescaler(u32_prescaler),
          m_u16_arr(u16_arr),
          m_bo_initialized(false)
      {
        map_tim_handles_to_object[&m_timer_handle] = this;
      }

    int32_t STM32PWM_IC::init()
    {
        /* USER CODE BEGIN TIM2_Init 0 */
        if (m_bo_initialized)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        /* USER CODE END TIM2_Init 0 */

        TIM_SlaveConfigTypeDef sSlaveConfig = {0};
        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIM_IC_InitTypeDef sConfigIC = {0};

        /* USER CODE BEGIN TIM2_Init 1 */

        /* USER CODE END TIM2_Init 1 */
        m_timer_handle.Instance = m_pt_timer_unit;
        m_timer_handle.Init.Prescaler = 0;
        m_timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
        m_timer_handle.Init.Period = m_u16_arr - 1;
        m_timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        m_timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_Base_Init(&m_timer_handle) != HAL_OK)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        if (HAL_TIM_IC_Init(&m_timer_handle) != HAL_OK)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
        sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
        sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
        sSlaveConfig.TriggerFilter = 0;
        if (HAL_TIM_SlaveConfigSynchronization(&m_timer_handle, &sSlaveConfig) != HAL_OK)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&m_timer_handle, &sMasterConfig) != HAL_OK)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
        sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
        sConfigIC.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&m_timer_handle, &sConfigIC, m_u32_first_channel) != HAL_OK)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
        if (HAL_TIM_IC_ConfigChannel(&m_timer_handle, &sConfigIC, m_u32_second_channel) != HAL_OK)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        /* USER CODE BEGIN TIM2_Init 2 */

        /* USER CODE END TIM2_Init 2 */
        m_bo_initialized = true;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t STM32PWM_IC::deinit()
    {
        if(m_bo_initialized != true)
        {
            return OSServices::ERROR_CODE_UNINITIALIZED;
        }
        m_bo_initialized = false;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t STM32PWM_IC::read_frequency_and_duty_cycle(uint32_t &u32_frequency_in_milihz, uint32_t &u32_duty_cycle_permil)
    {
        if (false == m_bo_initialized)
        {
            return OSServices::ERROR_CODE_UNINITIALIZED;
        }
        u32_frequency_in_milihz = m_u32_frequency_in_milihz;
        u32_duty_cycle_permil = m_u32_duty_cycle_permil;
        return OSServices::ERROR_CODE_SUCCESS;

    }

    void STM32PWM_IC::process_capture_callback(TIM_HandleTypeDef *htim)
    {
        // HAL_TIM_ACTIVE_CHANNEL_2
        HAL_TIM_ActiveChannel en_expected_channel = HAL_TIM_ACTIVE_CHANNEL_1;
        switch (m_u32_first_channel)
        {
        case TIM_CHANNEL_2:
            en_expected_channel = HAL_TIM_ACTIVE_CHANNEL_2;
            break;
        case TIM_CHANNEL_3:
            en_expected_channel = HAL_TIM_ACTIVE_CHANNEL_3;
            break;
        }

        if (htim->Channel == en_expected_channel)  // rising edge interrupt
        {
            // read captured value
            uint32_t IC_Val1 = HAL_TIM_ReadCapturedValue(htim, m_u32_first_channel);  // first value

            if (IC_Val1 != 0u)  // if the value is not 0
            {
                // read second value
                uint32_t IC_Val2 = HAL_TIM_ReadCapturedValue(htim, m_u32_second_channel); // falling edge value

                // calculate Duty cycle
                m_u32_duty_cycle_permil = (IC_Val2*1000/IC_Val1);

                // calculate frequency

                m_u32_frequency_in_milihz = (2*HAL_RCC_GetPCLK1Freq() * 1000 / IC_Val1);
                // As my timer2 clock is 2X the PCLK1 CLOCK that's why X2.
            }
            else
            {
                m_u32_duty_cycle_permil = 0u;
                m_u32_frequency_in_milihz = 0u;
            }
        }
    }

    void STM32PWM_IC::Error_Handler(void)
    {
        /* User may add here some code to deal with this error */
        while(1)
        {
          HAL_Delay(1000);
        }
    }
}


extern "C"
{
    void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
    {
        drivers::STM32PWM_IC* po_pwm_ic = map_tim_handles_to_object[htim];
        if (nullptr != po_pwm_ic)
        {
            po_pwm_ic->process_capture_callback(htim);
        }

    }


}
