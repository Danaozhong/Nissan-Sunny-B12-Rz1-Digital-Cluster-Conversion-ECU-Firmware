
#include <cstdio>

#include "watchdog.hpp"

namespace midware
{
    
    void Watchdog::activate(uint32_t num_of_ms)
    {
        uint8_t prescale_reg;
        uint8_t prescale_val;

        if (num_of_ms < 1)
        {
            num_of_ms = 1;
            prescale_reg = IWDG_PRESCALER_32;
            prescale_val = 1;
        }
        else if (num_of_ms <= 4096)
        {
            prescale_reg = IWDG_PRESCALER_32;
            prescale_val = 1;
        }
        else if (num_of_ms <= 8192)
        {
            prescale_reg = IWDG_PRESCALER_64;
            prescale_val = 2;
        }
        else if (num_of_ms <= 16384)
        {
            prescale_reg = IWDG_PRESCALER_128;
            prescale_val = 4;
        }
        else if (num_of_ms <= 32768)
        {
            prescale_reg = IWDG_PRESCALER_256;
            prescale_val = 8;
        }
        else
        {
            num_of_ms = 32768;
            prescale_reg = IWDG_PRESCALER_256;
            prescale_val = 8;
        }

  /*##-3- Configure & Start the IWDG peripheral #########################################*/
  /* Set counter reload value to obtain 1 sec. IWDG TimeOut.
     IWDG counter clock Frequency = uwLsiFreq
     Set Prescaler to 32 (IWDG_PRESCALER_32)
     Timeout Period = (Reload Counter Value * 32) / uwLsiFreq
     So Set Reload Counter Value = (1 * uwLsiFreq) / 32 */
      m_o_handle.Instance = IWDG;
      m_o_handle.Init.Prescaler = prescale_reg;
      m_o_handle.Init.Reload = (num_of_ms/prescale_val-1);
      m_o_handle.Init.Window = IWDG_WINDOW_DISABLE;

      if(HAL_IWDG_Init(&m_o_handle) != HAL_OK)
      {
        /* Initialization Error */
      }

    }

    void Watchdog::trigger()
    {
        if (m_bo_trigger_reset)
        {
            return;
        }
        
        if (HAL_IWDG_Refresh(&m_o_handle) != HAL_OK)
        {
            printf("Triggering wdog failed");
          //Error_Handler();
        }
    }
    
    void Watchdog::trigger_reset()
    {
        m_bo_trigger_reset = true;
    }
}
