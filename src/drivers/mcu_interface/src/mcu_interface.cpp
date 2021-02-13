#include "mcu_interface.hpp"
#include "stm32fxxx.h"


namespace drivers
{
    void McuInterface::configure_system_clock()
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
    
    uint32_t McuInterface::u32_get_sysclock() const
    {
        return HAL_RCC_GetHCLKFreq();
    }
    
    SystemResetReason McuInterface::get_reset_reason() const
    {
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET || __HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
        {
            // watchdog reset
            return RESET_REASON_WATCHDOG_RESET;
        }
        
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
        {
            return RESET_REASON_SOFTWARE_RESET;
        }
        
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)
        {
            return RESET_REASON_POWER_FAIL;
        }
        
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
        {
            return RESET_REASON_POWER_ON_RESET;
        }
        
        // unknown reset reason
        return RESET_REASON_UNKNOWN;
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
    void McuInterface::SystemClock_Config_STM32_F3_DISCOVERY(void)
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

#ifdef USE_STM32F3XX_NUCLEO_32
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
    void McuInterface::SystemClock_Config_STM32F303_NUCLEO_32(void)
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

#ifdef STM32F303xC
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
    void McuInterface::SystemClock_Config_STM32F303xC(void)
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
#endif /* STM32F303xC */
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
    void McuInterface::SystemClock_Config_STM32F429xx(void)
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
}
