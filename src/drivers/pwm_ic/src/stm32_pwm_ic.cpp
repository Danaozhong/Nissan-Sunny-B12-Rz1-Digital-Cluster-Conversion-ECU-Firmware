#include "stm32_pwm_ic.hpp"
#include "os_console.hpp"

#include <map>

/** Specifies how long to wait for a capture callback until assuming that there is no input data anymore. */
#define STM32_IC_MAXIMUM_TIME_WITHOUT_READING (200u)

namespace
{
    std::map<const TIM_HandleTypeDef*, drivers::STM32PWM_IC*> map_tim_handles_to_object;
    std::map<int32_t, TIM_HandleTypeDef*> map_timer_id_to_tim_handle;
}
namespace drivers
{

    //STM32PWM_IC::STM32PWM_IC(TIM_TypeDef* pt_timer_unit, uint32_t u16pre_scaler, uint8_t u16_arr)
    STM32PWM_IC::STM32PWM_IC(TIM_TypeDef* pt_timer_unit, uint32_t u32_first_channel, uint32_t u32_second_channel, uint16_t u16_prescaler, uint16_t u16_arr)
        : m_pt_timer_unit(pt_timer_unit),
          m_u32_first_channel(u32_first_channel),
          m_u32_second_channel(u32_second_channel),
          m_u16_prescaler(u16_prescaler),
          m_u16_arr(u16_arr),
          m_bo_initialized(false),
          m_p_callback(nullptr)
      {
        memset(&m_timer_handle, 0, sizeof(m_timer_handle));

        m_o_last_input_capture_timestamp = 0u;

        int32_t i32_timer_id = -1;
        if (TIM1 == pt_timer_unit)
        {
            i32_timer_id = 1;
        }
        else if (TIM2 == pt_timer_unit)
        {
            i32_timer_id = 2;
        }
        else if (TIM3 == pt_timer_unit)
        {
            i32_timer_id = 3;
        }
        else if (TIM4 == pt_timer_unit)
        {
            i32_timer_id = 4;
        }
        else if (TIM8 == pt_timer_unit)
        {
            i32_timer_id = 8;
        }
        
        if(i32_timer_id >= 0)
        {
            // the map_tim_handles_to_object maps from pointers of class members to the class instance
            map_tim_handles_to_object[&m_timer_handle] = this;
        
            map_timer_id_to_tim_handle[i32_timer_id] = &m_timer_handle;
        }
    }

    int32_t STM32PWM_IC::init()
    {
        /* Check if the driver is already initialized */
        if (m_bo_initialized)
        {
            return OSServices::ERROR_CODE_INTERNAL_ERROR;
        }
        
        /* make sure the parameters make sense */
        if(0 == m_u16_arr || 0 == m_u16_prescaler || nullptr == m_pt_timer_unit)
        {
            return OSServices::ERROR_CODE_PARAMETER_WRONG;
        }
        m_o_last_input_capture_timestamp = 0u;
        
        /* USER CODE END TIM2_Init 0 */
        set_prescaler(m_u16_prescaler);

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

        if (HAL_GetTick() - m_o_last_input_capture_timestamp < STM32_IC_MAXIMUM_TIME_WITHOUT_READING)
        {
            u32_frequency_in_milihz = m_u32_frequency_in_milihz;
            u32_duty_cycle_permil = m_u32_duty_cycle_permil;
        }
        else
        {
            u32_frequency_in_milihz = 0u;
            u32_duty_cycle_permil = 0u;
        }
        return OSServices::ERROR_CODE_SUCCESS;

    }

    int32_t STM32PWM_IC::set_capture_callback(void(*p_callback)(GenericPWM_IC*, uint32_t, uint32_t))
    {
        m_p_callback = p_callback;
        return OSServices::ERROR_CODE_SUCCESS;
    }

    int32_t STM32PWM_IC::set_prescaler(uint16_t u16_prescaler)
    {
        if (m_u16_prescaler != u16_prescaler || false == m_bo_initialized)
        {
            m_u16_prescaler = u16_prescaler;
    
            if (true == m_bo_initialized)
            {
                // deinitialize the current configuration
                if (HAL_TIM_IC_DeInit(&m_timer_handle) != HAL_OK)
                {
                    return OSServices::ERROR_CODE_INTERNAL_ERROR;
                }

                if (HAL_TIM_Base_DeInit(&m_timer_handle) != HAL_OK)
                {
                    return OSServices::ERROR_CODE_INTERNAL_ERROR;
                }
            }

            TIM_SlaveConfigTypeDef sSlaveConfig = {0};
            TIM_IC_InitTypeDef sConfigIC = {0};

            m_timer_handle.Instance = m_pt_timer_unit;
            m_timer_handle.Init.Prescaler = m_u16_prescaler;
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

            sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
            sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
            sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
            sConfigIC.ICFilter = 0;
            if (HAL_TIM_IC_ConfigChannel(&m_timer_handle, &sConfigIC, m_u32_first_channel) != HAL_OK)
            {
                return OSServices::ERROR_CODE_INTERNAL_ERROR;
            }
            sConfigIC.ICPolarity =TIM_INPUTCHANNELPOLARITY_FALLING;
            sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
            if (HAL_TIM_IC_ConfigChannel(&m_timer_handle, &sConfigIC, m_u32_second_channel) != HAL_OK)
            {
                return OSServices::ERROR_CODE_INTERNAL_ERROR;
            }

            sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
            sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
            sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
            sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
            sSlaveConfig.TriggerFilter = 0;
            if (HAL_TIM_SlaveConfigSynchro(&m_timer_handle, &sSlaveConfig) != HAL_OK)
            {
                return OSServices::ERROR_CODE_INTERNAL_ERROR;
            }
            // start the input capture in interrupt mode
            if (HAL_TIM_IC_Start_IT(&m_timer_handle, m_u32_first_channel) != HAL_OK)
            {
                return OSServices::ERROR_CODE_INTERNAL_ERROR;
            }
            if (HAL_TIM_IC_Start_IT(&m_timer_handle, m_u32_second_channel) != HAL_OK)
            {
                return OSServices::ERROR_CODE_INTERNAL_ERROR;
            }
        }
        return OSServices::ERROR_CODE_SUCCESS;
    }

    void STM32PWM_IC::process_capture_callback(TIM_HandleTypeDef *htim)
    {
        // check from which timer unit this callback is coming.
        HAL_TIM_ActiveChannel en_expected_channel = HAL_TIM_ACTIVE_CHANNEL_1;
        switch (m_u32_first_channel)
        {
        case TIM_CHANNEL_1:
            en_expected_channel = HAL_TIM_ACTIVE_CHANNEL_1;
            break;
        case TIM_CHANNEL_2:
            en_expected_channel = HAL_TIM_ACTIVE_CHANNEL_2;
            break;
        case TIM_CHANNEL_3:
            en_expected_channel = HAL_TIM_ACTIVE_CHANNEL_3;
            break;
        default:
            // unexpected channel used!
            return;
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
                // As my timer clock is 2X the PCLK1 CLOCK that's why X2.
                m_u32_frequency_in_milihz = ((2000ull*static_cast<uint64_t>(HAL_RCC_GetPCLK1Freq())) / (static_cast<uint64_t>(IC_Val1) * (static_cast<uint64_t>(this->m_u16_prescaler) + 1ull)));
            }
            else
            {
                m_u32_duty_cycle_permil = 0u;
                m_u32_frequency_in_milihz = 0u;
            }

            // trigger user-configurable callback, if used
            if (nullptr != m_p_callback)
            {
                m_p_callback(this, m_u32_frequency_in_milihz, m_u32_duty_cycle_permil);
            }

            // save a timestamp when the last input capture was done
            m_o_last_input_capture_timestamp = HAL_GetTick();
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

    void TIM4_IRQHandler(void)
    {
      /* USER CODE BEGIN TIM2_IRQn 0 */
      TIM_HandleTypeDef *po_timer_handle = nullptr;
      po_timer_handle = map_timer_id_to_tim_handle[4];
        if (nullptr != po_timer_handle)
        {
        /* USER CODE END TIM2_IRQn 0 */
          HAL_TIM_IRQHandler(po_timer_handle);
        }
    }

    void TIM8_IRQHandler(void)
    {
      /* USER CODE BEGIN TIM2_IRQn 0 */
      TIM_HandleTypeDef *po_timer_handle = nullptr;
      po_timer_handle = map_timer_id_to_tim_handle[8];
        if (nullptr != po_timer_handle)
        {
        /* USER CODE END TIM2_IRQn 0 */
          HAL_TIM_IRQHandler(po_timer_handle);
        }
    }

}
