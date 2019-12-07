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

#include "os_console.hpp"
#include "ex_thread.hpp"

/** @addtogroup STM32F3xx_HAL_Examples
  * @{
  */

/** @addtogroup DAC_SimpleConversion
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
#define  PERIOD_VALUE       (uint32_t)(665 - 1)  /* Period Value  */
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        /* Capture Compare 1 Value  */
/* Private define ------------------------------------------------------------*/
#define ADCCONVERTEDVALUES_BUFFER_SIZE 256                 /* Size of array aADCxConvertedValues[] */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#ifdef HAL_TIM_MODULE_ENABLED
/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;
/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;
/* Counter Prescaler value */
uint32_t uhPrescalerValue = 0;
#endif

/* ADC handler declaration */


/* Variable containing ADC conversions results */
__IO uint16_t   aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SystemClock_Config_STM32_F3_DISCOVERY(void);
static void SystemClock_Config_STM32F303_NUCLEO_32(void);

static void Error_Handler(void);
static void ADC_Config(void);

#ifdef HAL_ADC_MODULE_ENABLED
/* Private functions ---------------------------------------------------------*/
ADC_HandleTypeDef    AdcHandle;
#endif
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

/** This simple function initializes a PWM to simulate the clusters speedo signal sensor */
void main_speedo_sensor_pwm(void)
{
#ifdef HAL_TIM_MODULE_ENABLED
 /* Compute the prescaler value to have TIM3 counter clock equal to 24000000 Hz */
 // uhPrescalerValue = (uint32_t)(SystemCoreClock / 24000000) - 1;

 // 350 Hz
 //uhPrescalerValue = (uint32_t)(SystemCoreClock / 240000) - 1;

// 3.5 Hz
 //uhPrescalerValue = (uint32_t)(SystemCoreClock / 2400) - 1; // 2km/h 3,6Hz
 //uhPrescalerValue = (uint32_t)(SystemCoreClock / 24000) - 1; // 10km/h 36,1 Hz
 //uhPrescalerValue = (uint32_t)(SystemCoreClock / 240000) - 1; // 86km/h -- 360,9Hz
 //uhPrescalerValue = (uint32_t)(SystemCoreClock / 480000) - 1; // 171km/h  -- 721Hz
 //uhPrescalerValue = (uint32_t)(SystemCoreClock / 960000) - 1; // 180km/h/h  -- 1444kHz
 uhPrescalerValue = (uint32_t)(SystemCoreClock / 1000000) - 1; // 171km/h  -- 721Hz
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
  TimHandle.Instance = TIMx;

  TimHandle.Init.Prescaler         = uhPrescalerValue;
  TimHandle.Init.Period            = PERIOD_VALUE;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
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
  sConfig.Pulse = PULSE1_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }


  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }

  while(true)
  {
	  DEBUG_PRINTF("Log output via uart!");
		std_ex::sleep_for(std::chrono::milliseconds(1000));
  }
#endif
}



class MainApplication
{
public:
	MainApplication()
	{
#ifndef HAL_ADC_MODULE_ENABLED
#define ADC_CHANNEL_2 2
#endif
		// create the low-level hardware interfaces
		m_p_adc = new drivers::STM32ADC(drivers::ADCResolution::ADC_RESOLUTION_12BIT, ADC2, ADC_CHANNEL_2, GPIOA, GPIO_PIN_5);
		m_p_dac = new drivers::STM32DAC(DAC1, GPIOA, GPIO_PIN_4);

		/* Characteristics of the Nissan Sunny EUDM fuel sensor. 0% = 100Ohm (empty), 100% = 10Ohm (full). See
		 * http://texelography.com/2019/06/21/nissan-rz1-digital-cluster-conversion/ for the full dataset */
		std::pair<int32_t, int32_t> a_input_lut[] =
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

		m_p_o_fuel_gauge_input_characteristic = new app::CharacteristicCurve<int32_t, int32_t>(a_input_lut, sizeof(a_input_lut) / sizeof(a_input_lut[0]));

		/* Characteristics of the digital cluster fuel gauge */
		std::pair<int32_t, int32_t> a_output_lut[] =
		{
				std::make_pair(-1000, 5000),
				std::make_pair(0, 5000),
				std::make_pair(100, 4900),
				//std::make_pair(0, 4700),
				std::make_pair(714, 4340),
				std::make_pair(2143, 4040),
				std::make_pair(4286, 3300),
				std::make_pair(6429, 2240),
				std::make_pair(9286, 1100),
				std::make_pair(10000, 700),
				std::make_pair(11000, 700)
		};

		m_p_o_fuel_gauge_output_characteristic = new app::CharacteristicCurve<int32_t, int32_t>(a_output_lut, sizeof(a_output_lut) / sizeof(a_output_lut[0]));

		// start the data output thread
		//m_p_o_fuel_gauge_output = std::make_shared<app::FuelGaugeOutput>(m_p_dac, m_p_o_fuel_gauge_output_characteristic, 1500, 0);
		// start the data acquisition thread
		//m_p_o_fuel_gauge_input = std::make_shared<app::FuelGaugeInputFromADC>(m_p_adc, m_p_o_fuel_gauge_input_characteristic);

		// attach to the signal of the fuel sensor input
		//auto event_handler = std::bind(&MainApplication::fuel_sensor_input_received, this, std::placeholders::_1);
		//m_p_o_fuel_gauge_input->m_sig_fuel_level_changed.connect(event_handler);
	}

	void fuel_sensor_input_received(int32_t i32_value)
	{
		//if (m_p_o_fuel_gauge_output != nullptr)
		{
			//m_p_o_fuel_gauge_output->set_fuel_level(i32_value);
		}
	}
private:

	// prevent copying
	MainApplication(MainApplication &other) = delete;

	drivers::GenericADC* m_p_adc;
	drivers::GenericDAC* m_p_dac;

	app::CharacteristicCurve<int32_t, int32_t>* m_p_o_fuel_gauge_input_characteristic;
	app::CharacteristicCurve<int32_t, int32_t>* m_p_o_fuel_gauge_output_characteristic;

	app::FuelGaugeInputFromADC* m_p_o_fuel_gauge_input;
	app::FuelGaugeOutput* m_p_o_fuel_gauge_output;
};


void MAIN_startup_thread(void*)
{
	// This is the initial thread running after bootup.
	/* Configure the system clock to 64 MHz */
	SystemClock_Config();


	/* Configure LEDs */
#ifdef USE_STM32_F3_DISCO
	BSP_LED_Init(LED_RED);
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_BLUE);
#elif defined USE_STM32F3XX_NUCLEO_32
	BSP_LED_Init(LED_GREEN);
#endif


	// wait for the scheduler to be ready.
	std_ex::sleep_for(std::chrono::milliseconds(400));

	// create the debug interface
#ifdef USE_STM32_F3_DISCO
	std::shared_ptr<drivers::GenericUART> p_uart = std::make_shared<drivers::STM32HardwareUART>(GPIOD, GPIO_PIN_6, GPIOD, GPIO_PIN_5);
#elif defined USE_STM32F3XX_NUCLEO_32
	drivers::GenericUART* p_uart = new drivers::STM32HardwareUART(GPIOA, GPIO_PIN_3, GPIOA, GPIO_PIN_2);
#endif

	// ... and connect it to the hardware ports
	p_uart->connect(9600, drivers::UART_WORD_LENGTH_8BIT, drivers::UART_STOP_BITS_1, drivers::UART_FLOW_CONTROL_NONE);

	// if this was all successful, create the OS helper services
	//OSServices::OSConsole o_os_console(p_uart);
	set_serial_output(p_uart);

	// integrate all your tasks here.
	MainApplication o_application;

	//std_ex::thread o_thread_pwm(main_speedo_sensor_pwm);

	while (true)
	{
		// load balancing
		std_ex::sleep_for(std::chrono::milliseconds(1000));
		DEBUG_PRINTF("I am still here!");

		// check for debug input
		//o_os_console.run();
	}
	vTaskDelete(NULL);
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

	BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

	// first thread still needs to be created with xTaskCreate, only after the scheduler has started, std::thread can be used.

    xTaskCreate( MAIN_startup_thread,
                 "MAIN_startup_thread",
                 1024,
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
}


static void SystemClock_Config(void)
{

#ifdef USE_STM32F3_DISCO
	SystemClock_Config_STM32_F3_DISCOVERY();
#elif defined USE_STM32F3XX_NUCLEO_32
	SystemClock_Config_STM32F303_NUCLEO_32();
#else
#error "Please specify the system clock configuration for your target board."
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
static void SystemClock_Config_STM32_F3_DISCOVERY(void)
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

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 64000000
  *            HCLK(Hz)                       = 64000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            PLLMUL                         = RCC_PLL_MUL16 (16)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config_STM32F303_NUCLEO_32(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* HSI Oscillator already ON after system reset, activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
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
    /* Initialization Error */
    while(1);
  }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED3 on: Transfer error in reception/transmission process */
#ifdef USE_STM32_F3_DISCO
  BSP_LED_On(LED_RED);
#elif defined USE_STM32F3XX_NUCLEO_32
  BSP_LED_On(LED_GREEN);
#endif

}


/**
  * @brief  ADC configuration
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
#ifdef HAL_ADC_MODULE_ENABLED
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
#endif
}


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
  /* In case of ADC error, call main error handler */
  Error_Handler();
}
#endif


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
#ifdef USE_STM32_F3_DISCO
		BSP_LED_Toggle(LED_RED);
#elif defined USE_STM32F3XX_NUCLEO_32
		BSP_LED_Toggle(LED_GREEN);
#endif
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
