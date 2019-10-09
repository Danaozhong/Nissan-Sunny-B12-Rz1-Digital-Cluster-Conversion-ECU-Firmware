#include "stm32_adc.hpp"


#define ADCx_FORCE_RESET()              __HAL_RCC_ADC2_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC2_RELEASE_RESET()


namespace drivers
{
	/** Parameter constructor */
	STM32ADC::STM32ADC(ADCResolution en_resolution, ADC_TypeDef* pt_adc_peripheral, uint32_t u32_adc_channel, GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin)

	//STM32ADC::STM32ADC(ADCResolution en_resolution, ADC_TypeDef* pt_adc_peripheral, GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin)
		: m_en_adc_resolution(en_resolution), m_u32_adc_channel(u32_adc_channel),
		  m_u16_gpio_pin(u16_gpio_pin), m_pt_gpio_block(pt_gpio_block)
	{

		GPIO_InitTypeDef          GPIO_InitStruct;
		static DMA_HandleTypeDef  DmaHandle;
		RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;

		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable clock of GPIO associated to the peripheral channels */
		if (GPIOA == pt_gpio_block)
		{
			__HAL_RCC_GPIOA_CLK_ENABLE();
		}

		/* Enable clock of ADCx peripheral */
		if (ADC1 == pt_adc_peripheral)
		{
			__HAL_RCC_ADC1_CLK_ENABLE();
		}
		else if (ADC2 == pt_adc_peripheral)
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

		/* Enable asynchronous clock source of ADCx */
		RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
		RCC_PeriphCLKInitStruct.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
		HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

		/* Enable clock of DMA associated to the peripheral */
		//ADCx_DMA_CLK_ENABLE();

		/*##-2- Configure peripheral GPIO ##########################################*/
		/* Configure GPIO pin of the selected ADC channel */
		GPIO_InitStruct.Pin = m_u16_gpio_pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(m_pt_gpio_block, &GPIO_InitStruct);

		/* NVIC configuration for ADC interrupt */
		/* Priority: high-priority */
		HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

		/* Configuration of ADCx init structure: ADC parameters and regular group */
		m_adc_handle.Instance = pt_adc_peripheral;

		m_adc_handle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
		if (ADC_RESOLUTION_12BIT == m_en_adc_resolution)
		{
			m_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
		}
		m_adc_handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
		m_adc_handle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
		m_adc_handle.Init.EOCSelection          = ADC_EOC_SEQ_CONV;
		m_adc_handle.Init.LowPowerAutoWait      = DISABLE;
		m_adc_handle.Init.ContinuousConvMode    = ENABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
		m_adc_handle.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
		m_adc_handle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
		m_adc_handle.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
		m_adc_handle.Init.ExternalTrigConv      =  ADC_SOFTWARE_START;   // ???
		m_adc_handle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
		m_adc_handle.Init.DMAContinuousRequests = DISABLE;
		m_adc_handle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;

		if (HAL_ADC_Init(&m_adc_handle) != HAL_OK)
		{
			/* ADC initialization error */
			Error_Handler();
	    }

		ADC_ChannelConfTypeDef   sConfig;
		/* Configuration of channel on ADCx regular group on sequencer rank 1 */
		/* Note: Considering IT occurring after each ADC conversion if ADC          */
		/*       conversion is out of the analog watchdog widow selected (ADC IT    */
		/*       enabled), select sampling time and ADC clock with sufficient       */
		/*       duration to not create an overhead situation in IRQHandler.        */
		sConfig.Channel      = m_u32_adc_channel;
		sConfig.Rank         = ADC_REGULAR_RANK_1;
		sConfig.SamplingTime = ADC_SAMPLETIME_181CYCLES_5;
		sConfig.SingleDiff   = ADC_SINGLE_ENDED;
		sConfig.OffsetNumber = ADC_OFFSET_NONE;
		sConfig.Offset       = 0;

		if (HAL_ADC_ConfigChannel(&m_adc_handle, &sConfig) != HAL_OK)
		{
			/* Channel Configuration Error */
			Error_Handler();
		}

		/* Run the ADC calibration in single-ended mode */
		if (HAL_ADCEx_Calibration_Start(&m_adc_handle, ADC_SINGLE_ENDED) != HAL_OK)
		{
			/* Calibration Error */
			Error_Handler();
		}

		HAL_ADC_Start(&m_adc_handle);



	}

	/** Destructor. */
	STM32ADC::~STM32ADC()
	{
		  /*##-1- Reset peripherals ##################################################*/
		  ADCx_FORCE_RESET();
		  ADCx_RELEASE_RESET();

		  /*##-2- Disable peripherals and GPIO Clocks ################################*/
		  /* De-initialize GPIO pin of the selected ADC channel */
		  HAL_GPIO_DeInit(m_pt_gpio_block, m_u16_gpio_pin);

		  /* Disable the NVIC configuration for ADC interrupt */
		  HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
	}

	uint32_t STM32ADC::get_adc_min_value() const
	{
			return 0u;

	}

	uint32_t STM32ADC::get_adc_max_value() const
	{
		return 0xFFFFFF;
	}

	uint32_t STM32ADC::read_adc_value()
	{
        if (HAL_ADC_PollForConversion(&m_adc_handle, 1) == HAL_OK)
        {
            return HAL_ADC_GetValue(&m_adc_handle);
        }
        return 0u;
	}

	void STM32ADC::Error_Handler(void)
	{
	  /* User may add here some code to deal with this error */
	  while(1)
	  {
	    BSP_LED_Toggle(LED_RED);
	    HAL_Delay(1000);
	  }
	}
}
