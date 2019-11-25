/**
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/Src/main.c 
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use GPIOs through 
  *          the STM32F3xx HAL API.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

/* OS headers */
#include "FreeRTOS.h"
#include "task.h"


/* Own headers */
#include "fuel_gauge_input.hpp"
#include "fuel_gauge_output.hpp"
#include "lookup_table.hpp"
#include "main.h"
#include "stm32_dac.hpp"
#include "stm32_adc.hpp"
#include "stm32_uart.hpp"
#include "thread.hpp"

#include "os_console.hpp"


/** @addtogroup STM32F3xx_HAL_Examples
  * @{
  */

/** @addtogroup DAC_SimpleConversion
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADCCONVERTEDVALUES_BUFFER_SIZE 256                 /* Size of array aADCxConvertedValues[] */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Stuff related to CAN */
CAN_HandleTypeDef     CanHandle;
CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;


/* ADC handler declaration */


/* Variable containing ADC conversions results */
__IO uint16_t   aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void ADC_Config(void);
static void CAN_Config(void);


/* Private functions ---------------------------------------------------------*/
ADC_HandleTypeDef    AdcHandle;
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

extern "C"
{

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

	void vApplicationIdleHook( void )
	{
		/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
		to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
		task.  It is essential that code added to this hook function never attempts
		to block in any way (for example, call xQueueReceive() with a block time
		specified, or call vTaskDelay()).  If the application makes use of the
		vTaskDelete() API function (as this demo application does) then it is also
		important that vApplicationIdleHook() is permitted to return to its calling
		function, because it is the responsibility of the idle task to clean up
		memory allocated by the kernel to any task that has since been deleted. */
	}

	/** see https://www.cnblogs.com/shangdawei/p/4684798.html */
	void vApplicationTickHook(void)
	{
	  HAL_IncTick();
	}


	void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
	{
		( void ) pcTaskName;
		( void ) pxTask;

		/* Run time stack overflow checking is performed if
		configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
		function is called if a stack overflow is detected. */
		taskDISABLE_INTERRUPTS();
		for( ;; );
	}
}

class MainApplication
{
public:
	MainApplication()
	{
		// create the low-level hardware interfaces
		m_p_adc = std::make_shared<drivers::STM32ADC>(drivers::ADCResolution::ADC_RESOLUTION_12BIT, ADC2, ADC_CHANNEL_2, GPIOA, GPIO_PIN_5);
		m_p_dac = std::make_shared<drivers::STM32DAC>(DAC1, GPIOA, GPIO_PIN_4);
		/* Characteristics of the Nissan Sunny EUDM fuel sensor. 0% = 100Ohm (empty), 100% = 10Ohm (full). See
		 * http://texelography.com/2019/06/21/nissan-rz1-digital-cluster-conversion/ for the full dataset */
		std::vector<std::pair<int32_t, int32_t>> a_input_lut =
		{
				/* Fuel level (% * 100)  Resistor value in mOhm */
				std::make_pair(-1000, 120000),
				std::make_pair(-100, 87000),
				std::make_pair(500, 80600),
				std::make_pair(2500, 61800),
				std::make_pair(4800, 35700),
				std::make_pair(7700, 21000),
				std::make_pair(10000, 11800),
				std::make_pair(11000, 2400),
				std::make_pair(11500, 0000),
		};

		m_p_o_fuel_gauge_input_characteristic = std::make_shared<app::CharacteristicCurve<int32_t, int32_t>>(a_input_lut);

		/* Characteristics of the digital cluster fuel gauge */
		std::vector<std::pair<int32_t, double>> a_output_lut =
		{
				std::make_pair(-1000, 4.9),
				std::make_pair(0, 4.7),
				std::make_pair(714, 4.34),
				std::make_pair(2143, 4.04),
				std::make_pair(4286, 3.30),
				std::make_pair(6429, 2.24),
				std::make_pair(9286, 1.10),
				std::make_pair(10000, 0.7),
				std::make_pair(11000, 0.7)
		};

		m_p_o_fuel_gauge_output_characteristic = std::make_shared<app::CharacteristicCurve<int32_t, double>>(a_output_lut);

		// start the data output thread
		m_p_o_fuel_gauge_output = std::make_shared<app::FuelGaugeOutput>(m_p_dac, m_p_o_fuel_gauge_output_characteristic, 1.5, 0.0);
		// start the data acquisition thread
		m_p_o_fuel_gauge_input = std::make_shared<app::FuelGaugeInputFromADC>(m_p_adc, m_p_o_fuel_gauge_input_characteristic);

		// attach to the signal of the fuel sensor input
		auto event_handler = std::bind(&MainApplication::fuel_sensor_input_received, this, std::placeholders::_1);
		m_p_o_fuel_gauge_input->m_sig_fuel_level_changed.connect(event_handler);
	}

	void fuel_sensor_input_received(int32_t i32_value)
	{
		if (m_p_o_fuel_gauge_output != nullptr)
		{
			m_p_o_fuel_gauge_output->set_fuel_level(i32_value);
		}
	}
private:

	// prevent copying
	MainApplication(MainApplication &other) = delete;

	std::shared_ptr<drivers::GenericADC> m_p_adc;
	std::shared_ptr<drivers::GenericDAC> m_p_dac;

	std::shared_ptr<app::CharacteristicCurve<int32_t, int32_t>> m_p_o_fuel_gauge_input_characteristic;
	std::shared_ptr<app::CharacteristicCurve<int32_t, double>> m_p_o_fuel_gauge_output_characteristic;

	std::shared_ptr<app::FuelGaugeInputFromADC> m_p_o_fuel_gauge_input;
	std::shared_ptr<app::FuelGaugeOutput> m_p_o_fuel_gauge_output;
};


void MAIN_startup_thread(void*)
{
	// This is the initial thread running after bootup.

	// wait for the scheduler to be ready.
	std_ex::sleep_for(std::chrono::milliseconds(400));

	// create the debug interface
	std::shared_ptr<drivers::GenericUART> p_uart = std::make_shared<drivers::STM32HardwareUART>(GPIOD, GPIO_PIN_6, GPIOD, GPIO_PIN_5);

	// ... and connect it to the hardware ports
	p_uart->connect(9600, drivers::UART_WORD_LENGTH_8BIT, drivers::UART_STOP_BITS_1, drivers::UART_FLOW_CONTROL_NONE);

	// if this was all successful, create the OS helper services
	OSServices::OSConsole o_os_console(p_uart);
	set_serial_output(p_uart);

	// integrate all your tasks here.

	//drivers::STM32DAC o_stm32_dac(DAC1, GPIOA, GPIO_PIN_4);
	MainApplication o_application;


	while (true)
	{
		// load balancing
		std_ex::sleep_for(std::chrono::milliseconds(10));

		// check for debug input
		o_os_console.run();

#if 0
		if (p_uart->available() > 0)
		{
			int incomingByte = p_uart->read();
			if (incomingByte >= 0)
			{
				DEBUG_PRINTF("I received: %d", incomingByte);
			}
		}
#endif
		// listen on the serial input, etc.
		//char buffer[100] = "Test serial!";
		//p_uart->write(reinterpret_cast<uint8_t*>(buffer), 10);
	}
}


int main_can(void)
{
  /* Configure the CAN peripheral */
  CAN_Config();
  
 /* Set the data to be transmitted */
        TxData[0] = 0x00;
        TxData[1] = 0xAD;
        
        /* Start the Transmission process */
        if (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox) != HAL_OK)
        {
          /* Transmission request Error */
          Error_Handler();
        }
        HAL_Delay(10);
        return 0;
}



int main(void)
{


	/* STM32F3xx HAL library initialization:
	   - Configure the Flash prefetch
	   - Configure the Systick to generate an interrupt each 1 msec
	   - Set NVIC Group Priority to 4
	   - Low Level Initialization
	 */
	HAL_Init();

	/* Configure LED2 */
	BSP_LED_Init(LED_RED);
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_BLUE);

	/* Configure the system clock to 64 MHz */
	SystemClock_Config();

	BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

	// first thread still needs to be created with xTaskCreate, only after the scheduler has started, std::thread can be used.

    xTaskCreate( MAIN_startup_thread,
                 "MAIN_startup_thread",
                 0x600,
                 NULL,
                 2,
                 &xHandle );

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */

	for( ;; );
#if 0
  /* Configure the ADC peripheral */
  ADC_Config();

  /* Run the ADC calibration in single-ended mode */
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK)
  {
    /* Calibration Error */
    Error_Handler();
  }

  HAL_ADC_Start(&AdcHandle);
#endif
  /*##-1- Configure the DAC peripheral #######################################*/
#if 0
  DacHandle.Instance = DACx;

  if (HAL_DAC_Init(&DacHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure DAC channel1 #############################################*/
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

  if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DACx_CHANNEL) != HAL_OK)
  {
    /* Channel configuration Error */
    Error_Handler();
  }

  /*##-3- Set DAC Channel1 DHR register ######################################*/
  if (HAL_DAC_SetValue(&DacHandle, DACx_CHANNEL, DAC_ALIGN_8B_R, 0xA2) != HAL_OK)
  {
    /* Setting value Error */
    Error_Handler();
  }

  /*##-4- Enable DAC Channel1 ################################################*/
  if (HAL_DAC_Start(&DacHandle, DACx_CHANNEL) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }
#endif

  //drivers::STM32DAC o_stm32_dac(DAC1, GPIOA, GPIO_PIN_4);
 // drivers::STM32ADC o_stm32_adc(drivers::ADCResolution::ADC_RESOLUTION_12BIT, ADC2, ADC_CHANNEL_1, GPIOA, GPIO_PIN_4);


  /*## Start ADC conversions #################################################*/

#if 0
  /* Start ADC conversion on regular group with transfer by DMA */
  if (HAL_ADC_Start_DMA(&AdcHandle,
                        (uint32_t *)aADCxConvertedValues,
                        ADCCONVERTEDVALUES_BUFFER_SIZE
                       ) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }
#endif

#if 0
  /* Infinite loop */
  uint8_t u8_dac_value = 0u;
  uint32_t g_ADCValue = 0u;
  bool bo_ramup = true;
  while (1)
  {
	    BSP_LED_Toggle(LED_GREEN);

	    if (bo_ramup)
	    {
	    	u8_dac_value += 1;

			if (u8_dac_value == 255)
			{
				bo_ramup = false;
			}
	    }
	    else
	    {
	    	u8_dac_value -= 1;
			if (u8_dac_value == 0)
			{
				bo_ramup = true;
			}
	    }

	    o_stm32_dac.set_output_value(static_cast<uint32_t>(u8_dac_value));

	    HAL_Delay(5);

	    uint32_t u32_adc_value = o_stm32_adc.read_adc_value();

  }
#endif
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV                     = 1
  *            PLLMUL                         = RCC_PLL_MUL9 (9)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    Error_Handler();
  }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED3 on: Transfer error in reception/transmission process */
  BSP_LED_On(LED_RED);
}


/**
  * @brief  ADC configuration
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
  ADC_ChannelConfTypeDef   sConfig;




  /* Configuration of ADCx init structure: ADC parameters and regular group */
  AdcHandle.Instance = ADC2;

  AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.EOCSelection          = ADC_EOC_SEQ_CONV;
  AdcHandle.Init.LowPowerAutoWait      = DISABLE;
  AdcHandle.Init.ContinuousConvMode    = ENABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  AdcHandle.Init.NbrOfConversion       = 1;                             /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.ExternalTrigConv      =  ADC_SOFTWARE_START;   // ???
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  AdcHandle.Init.DMAContinuousRequests = DISABLE;
  AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* ADC initialization error */
    Error_Handler();
  }

  /* Configuration of channel on ADCx regular group on sequencer rank 1 */
  /* Note: Considering IT occurring after each ADC conversion if ADC          */
  /*       conversion is out of the analog watchdog widow selected (ADC IT    */
  /*       enabled), select sampling time and ADC clock with sufficient       */
  /*       duration to not create an overhead situation in IRQHandler.        */
  sConfig.Channel      = ADCx_CHANNELa;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_181CYCLES_5;
  sConfig.SingleDiff   = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset       = 0;

  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    Error_Handler();
  }

}


/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : AdcHandle handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
	BSP_LED_Toggle(LED_BLUE);
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
  /* In case of ADC error, call main error handler */
  Error_Handler();
}

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
static void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;

  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CANx;

  CanHandle.Init.TimeTriggeredMode = DISABLE;
  CanHandle.Init.AutoBusOff = DISABLE;
  CanHandle.Init.AutoWakeUp = DISABLE;
  CanHandle.Init.AutoRetransmission = ENABLE;
  CanHandle.Init.ReceiveFifoLocked = DISABLE;
  CanHandle.Init.TransmitFifoPriority = DISABLE;
  CanHandle.Init.Mode = CAN_MODE_NORMAL;
  CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
  CanHandle.Init.TimeSeg1 = CAN_BS1_9TQ;
  CanHandle.Init.TimeSeg2 = CAN_BS2_8TQ;
  CanHandle.Init.Prescaler = 2;

  if (HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&CanHandle) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x321;
  TxHeader.ExtId = 0x01;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;
}

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }

#if 0
  /* Display LEDx */
  if ((RxHeader.StdId == 0x321) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 2))
  {
    //LED_Display(RxData[0]);
    //ubKeyNumber = RxData[0];
  }
#endif
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
    BSP_LED_Toggle(LED_RED);
    HAL_Delay(1000);
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
