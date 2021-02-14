#include <cstring>

#include "stm32_adc.hpp"
#include "trace_if.h"

namespace drivers
{
    /** Parameter constructor */
    STM32ADC::STM32ADC(ADCResolution en_resolution, ADC_TypeDef* pt_adc_peripheral, uint32_t u32_adc_channel, GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin)
        : m_en_adc_resolution(en_resolution), m_u32_adc_channel(u32_adc_channel),
          m_pt_gpio_block(pt_gpio_block), m_u16_gpio_pin(u16_gpio_pin), m_pt_adc_peripheral(pt_adc_peripheral)
    {
        GPIO_InitTypeDef          GPIO_InitStruct;
        RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;

        TRACE_DECLARE_CONTEXT("ADC");

        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable clock of GPIO associated to the peripheral channels */
        if (GPIOA == pt_gpio_block)
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        }

        /* Enable clock of ADCx peripheral */
        if (ADC1 == m_pt_adc_peripheral)
        {
            __HAL_RCC_ADC1_CLK_ENABLE();
        }
        else if (ADC2 == m_pt_adc_peripheral)
        {
            __HAL_RCC_ADC2_CLK_ENABLE();
        }

        /* Note: In case of usage of asynchronous clock derived from ADC dedicated  */
        /*       PLL 72MHz, with ADC setting                                        */
        /*       "AdcHandle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1",                 */
        /*       the clock source has to be enabled at RCC top level using macro    */
        /*       "__HAL_RCC_ADC12_CONFIG(RCC_ADC12PLLCLK_DIV1)" or function         */
        /*       "HAL_RCCEx_PeriphCLKConfig()" (refer to comments in file           */
        /*       "stm32f3_hal_adc.c_ex" header).                                    */

#ifdef STM32_FAMILY_F3
        /* Enable asynchronous clock source of ADCx */
        RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
        RCC_PeriphCLKInitStruct.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
        HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
#endif

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* Configure GPIO pin of the selected ADC channel */
        GPIO_InitStruct.Pin = m_u16_gpio_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(m_pt_gpio_block, &GPIO_InitStruct);

#ifdef STM32_FAMILY_F3
        /* NVIC configuration for ADC interrupt */
        /* Priority: high-priority */
        HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
#endif

        /* Configuration of ADCx init structure: ADC parameters and regular group */
#ifdef HAL_ADC_MODULE_ENABLED
        std::memset(&m_adc_handle, 0, sizeof(m_adc_handle));
        m_adc_handle.State = 0u;
        m_adc_handle.Instance = pt_adc_peripheral;

        m_adc_handle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
        if (ADC_RESOLUTION_12BIT == m_en_adc_resolution)
        {
            m_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
        }
        m_adc_handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
        m_adc_handle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
        m_adc_handle.Init.EOCSelection          = ADC_EOC_SEQ_CONV;
#ifdef STM32_FAMILY_F3
        m_adc_handle.Init.LowPowerAutoWait      = DISABLE;
#endif
        m_adc_handle.Init.ContinuousConvMode    = ENABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
        m_adc_handle.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
        m_adc_handle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
        m_adc_handle.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
        m_adc_handle.Init.ExternalTrigConv      =  ADC_SOFTWARE_START;   // ???
        m_adc_handle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
        m_adc_handle.Init.DMAContinuousRequests = DISABLE;
#ifdef STM32_FAMILY_F3
        m_adc_handle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
#endif

        if (HAL_ADC_Init(&m_adc_handle) != HAL_OK)
        {
            /* ADC initialization error */
            TRACE_LOG("ADC", LOGLEVEL_ERROR, "ADC Initialization failed!");
            return;
        }

        ADC_ChannelConfTypeDef   sConfig;
        /* Configuration of channel on ADCx regular group on sequencer rank 1 */
        /* Note: Considering IT occurring after each ADC conversion if ADC          */
        /*       conversion is out of the analog watchdog widow selected (ADC IT    */
        /*       enabled), select sampling time and ADC clock with sufficient       */
        /*       duration to not create an overhead situation in IRQHandler.        */
        sConfig.Channel      = m_u32_adc_channel;

#ifdef STM32_FAMILY_F3
        sConfig.Rank         = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_181CYCLES_5;
        sConfig.SingleDiff   = ADC_SINGLE_ENDED;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
#elif defined (STM32_FAMILY_F4)
        sConfig.Rank         = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
#endif
        sConfig.Offset       = 0;

        if (HAL_ADC_ConfigChannel(&m_adc_handle, &sConfig) != HAL_OK)
        {
            /* Channel Configuration Error */
            TRACE_LOG("ADC", LOGLEVEL_ERROR, "ADC channel configuration failed!");
            return;
        }
#ifdef STM32_FAMILY_F3
        /* Run the ADC calibration in single-ended mode. Only needed for STM32F3 */
        if (HAL_ADCEx_Calibration_Start(&m_adc_handle, ADC_SINGLE_ENDED) != HAL_OK)
        {
            /* Calibration Error */
            TRACE_LOG("ADC", LOGLEVEL_ERROR, "ADC calibration failed!");
            return;
        }
#endif

        HAL_ADC_Start(&m_adc_handle);
#endif
    }

    /** Destructor. */
    STM32ADC::~STM32ADC()
    {
        /*##-1- Reset peripherals ##################################################*/
#ifdef STM32_FAMILY_F3
        if (ADC1 == m_pt_adc_peripheral)
        {
            __HAL_RCC_ADC1_FORCE_RESET();
            __HAL_RCC_ADC1_RELEASE_RESET();
        }
        else if (ADC2 == m_pt_adc_peripheral)
        {
            __HAL_RCC_ADC2_FORCE_RESET();
            __HAL_RCC_ADC2_RELEASE_RESET();
        }
#elif defined(STM32_FAMILY_F4)
        __HAL_RCC_ADC_FORCE_RESET()
        __HAL_RCC_ADC_RELEASE_RESET()
#endif
        /*##-2- Disable peripherals and GPIO Clocks ################################*/
        /* De-initialize GPIO pin of the selected ADC channel */
        HAL_GPIO_DeInit(m_pt_gpio_block, m_u16_gpio_pin);
#ifdef STM32_FAMILY_F3
        /* Disable the NVIC configuration for ADC interrupt */
        HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
#endif
    }

    uint32_t STM32ADC::get_adc_min_value() const
    {
        return 0u;
    }

    uint32_t STM32ADC::get_adc_max_value() const
    {
        // the maximum value depends on the configured resolution of the ADC
        if (ADC_RESOLUTION_12BIT == m_en_adc_resolution)
        {
            return 0x0FFF;
        }
        return 0xFFFF;
    }

    uint32_t STM32ADC::read_adc_value()
    {
#ifdef HAL_ADC_MODULE_ENABLED
        // trigger conversion
        if (HAL_ADC_PollForConversion(&m_adc_handle, 1) == HAL_OK)
        {
            // and return the read value
            return HAL_ADC_GetValue(&m_adc_handle);
        }
        else
        {
            TRACE_LOG("ADC", LOGLEVEL_ERROR, "ADC Conversion failed!");
        }
#endif
        return 0u;
    }
}

extern "C"
{
    
#ifdef HAL_ADC_MODULE_ENABLED
    /**
      * @brief  Conversion complete callback in non blocking mode
      * @param  AdcHandle : AdcHandle handle
      * @note   This example shows a simple way to report end of conversion
      *         and get conversion result. You can add your own implementation.
      * @retval None
      */
    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
    {

    }

    /**
      * @brief  Conversion DMA half-transfer callback in non blocking mode
      * @param  hadc: ADC handle
      * @retval None
      */
    void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
    {

    }

    /**
      * @brief  Analog watchdog callback in non blocking mode.
      * @note:  In case of several analog watchdog enabled, if needed to know
                which one triggered and on which ADCx, check Analog Watchdog flag
                ADC_FLAG_AWD1/2/3 into HAL_ADC_LevelOutOfWindowCallback() function.
                For example:"if (__HAL_ADC_GET_FLAG(hadc1, ADC_FLAG_AWD1) != RESET)"
      * @param  hadc: ADC handle
      * @retval None
      */
      void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
    {
    }

    /**
      * @brief  ADC error callback in non blocking mode
      *        (ADC conversion with interruption or transfer by DMA)
      * @param  hadc: ADC handle
      * @retval None
      */
    void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
    {
    }
#endif
}