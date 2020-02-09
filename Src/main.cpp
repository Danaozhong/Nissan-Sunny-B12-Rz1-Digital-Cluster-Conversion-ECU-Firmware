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
#include "main_application.hpp"
#include "uc_ports.hpp"


/** @addtogroup STM32F3xx_HAL_Examples
  * @{
  */

/* Private define ------------------------------------------------------------*/
#define ADCCONVERTEDVALUES_BUFFER_SIZE 256                 /* Size of array aADCxConvertedValues[] */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* ADC handler declaration */


/* Variable containing ADC conversions results */
__IO uint16_t   aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SystemClock_Config_STM32_F3_DISCOVERY(void);
static void SystemClock_Config_STM32F303_NUCLEO_32(void);
static void SystemClock_Config_STM32F303xC(void);
void SystemClock_Config_STM32F429xx(void);

static void Error_Handler(void);
//static void ADC_Config(void);

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
void* malloc (size_t size) // _NOTHROW
{
    /* Call the FreeRTOS version of malloc. */
    return pvPortMalloc( size );
}
void free (void* ptr)
{
    /* Call the FreeRTOS version of free. */
    vPortFree( ptr );
}

}
void * operator new( size_t size )
{
    return pvPortMalloc( size );
}

void * operator new[]( size_t size )
{
    return pvPortMalloc(size);
}

void operator delete( void * ptr )
{
    vPortFree ( ptr );
}

void operator delete[]( void * ptr )
{
    vPortFree ( ptr );
}

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


void MAIN_Cycle_100ms(void)
{
    app::MainApplication& o_application = app::MainApplication::get();
    int32_t counter = 0;

    while(true)
    {
        if (nullptr != o_application.m_p_o_fuel_gauge_input)
        {
            o_application.m_p_o_fuel_gauge_input->process_cycle();
        }

        if (nullptr != o_application.m_po_speed_sensor_converter)
        {
            o_application.m_po_speed_sensor_converter->cycle();
        }

        counter++;
        if (counter > 10)
        {
            counter = 0;
            // backup the shadow to flash every second
            o_application.get_nonvolatile_data_handler()->store();
        }

        std_ex::sleep_for(std::chrono::milliseconds(100));
    }
}


void MAIN_startup_thread(void)
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

	// and create our main application.
	app::MainApplication& o_application = app::MainApplication::get();
	o_application.startup_from_reset();

	// start the cyclic thread(s)
    std_ex::thread* cycle_100ms_thread = new std_ex::thread(&MAIN_Cycle_100ms, "Cycle100ms", 2u, 0x800);

    std::vector<uint32_t> a_freqs = { 1, 3, 5, 8, 220 };


	while (true)
	{
		// load balancing
		std_ex::sleep_for(std::chrono::milliseconds(10));

		// Check for data on the UART
		drivers::STM32HardwareUART* po_uart = static_cast<drivers::STM32HardwareUART*>(o_application.m_p_uart);
		if (nullptr != po_uart)
		{
		    po_uart->uart_process_cycle();
		}
		// check for debug input
		o_application.get_os_console()->run();
	}
	vTaskDelete(NULL);
}


int main(void)
{
    // create the port configuration object
    drivers::UcPorts* po_uc_port_configuration = new drivers::STM32F303CCT6UcPorts();

	/* STM32F3xx HAL library initialization:
	   - Configure the Flash prefetch
	   - Configure the Systick to generate an interrupt each 1 msec
	   - Set NVIC Group Priority to 4
	   - Low Level Initialization
	 */
	HAL_Init();

	TaskHandle_t xHandle = NULL;

	// first thread still needs to be created with xTaskCreate, only after the scheduler has started, std::thread can be used.

	std_ex::thread* startup_thread = new std_ex::thread(&MAIN_startup_thread, "Startup_Thread", 3u, 0x800);

#if 0
    xTaskCreate( MAIN_startup_thread,
                 "MAIN_startup_thread",
                 0x800,
                 NULL,
                 2,
                 &xHandle );
#endif

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
#elif defined STM32F303xC
	SystemClock_Config_STM32F303xC();
#elif defined STM32F429xx
	SystemClock_Config_STM32F429xx();
#else
#error "Please specify the system clock configuration for your target board."
#endif
}

#ifdef STM32_FAMILY_F3
#ifdef USE_STM32F3_DISCO
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
#endif
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
void SystemClock_Config_STM32F303xC(void)
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
#endif

#ifdef STM32_FAMILY_F4
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config_STM32F429xx(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Activate the Over-Drive mode */
  HAL_PWREx_EnableOverDrive();

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
#endif

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED3 on: Transfer error in reception/transmission process */
#ifdef USE_STM32_F3_DISCO
  BSP_LED_On(LED_RED);
#elif defined USE_STM32F3XX_NUCLEO_32
  BSP_LED_On(LED_GREEN);
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
