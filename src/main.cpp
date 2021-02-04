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
#ifdef USE_CAN
#include "CanIf.h"
#endif

#if 1
#include "trace_if.h"
#endif

/* OS headers */
#include "sysclock.hpp"
#include "uc_ports.hpp"
#include "main_application.hpp"


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

void MAIN_startup_thread(void*)
{
    // create our main application.
    app::MainApplication& o_application = app::MainApplication::get();
    o_application.startup_from_reset();
    o_application.console_thread();
    vTaskDelete(NULL);
}

int main(void)
{
    // configure the system clock
    drivers::SysClock system_clock;
    system_clock.configure();
    
    // create the port configuration object
    drivers::STM32F303CCT6UcPorts po_uc_port_configuration;
    po_uc_port_configuration.configure();
    
    HAL_Init();

    // first thread still needs to be created with xTaskCreate, only after the scheduler has started, std::thread can be used.
    TaskHandle_t xHandle = NULL;
    xTaskCreate( MAIN_startup_thread,
                 "MAIN_startup_thread",
                 0x800,
                 NULL,
                 3u,
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
