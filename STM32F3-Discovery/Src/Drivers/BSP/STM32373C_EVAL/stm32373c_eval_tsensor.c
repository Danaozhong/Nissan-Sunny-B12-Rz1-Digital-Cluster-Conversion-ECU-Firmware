/**
  ******************************************************************************
  * @file    stm32373c_eval_tsensor.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the I2C LM75 
  *          temperature sensor mounted on STM32373C-EVAL board . 
  *          It implements a high level communication layer for read and write 
  *          from/to this sensor. The needed STM323F37x hardware resources (I2C and 
  *          GPIO) are defined in stm32373c_eval.h file, and the initialization is 
  *          performed in TSENSOR_IO_Init() function declared in stm32373c_eval.c 
  *          file.
  *          You can easily tailor this driver to any other development board, 
  *          by just adapting the defines for hardware resources and 
  *          TSENSOR_IO_Init() function. 
  *
  *     +--------------------------------------------------------------------+
  *     |                        Pin assignment                              |                 
  *     +----------------------------------------+--------------+------------+
  *     |  STM32F37x I2C Pins                    |   STLM75     |   Pin      |
  *     +----------------------------------------+--------------+------------+
  *     | EVAL_I2C2_SDA_PIN                      |   SDA        |    1       |
  *     | EVAL_I2C2_SCL_PIN                      |   SCL        |    2       |
  *     | EVAL_I2C2_SMBUS_PIN                    |   OS/INT     |    3       |
  *     | .                                      |   GND        |    4 (0V)  |
  *     | .                                      |   A2         |    5       |
  *     | .                                      |   A1         |    6       |
  *     | .                                      |   A0         |    7       |
  *     | .                                      |   VDD        |    8 (5V)  |
  *     +----------------------------------------+--------------+------------+
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
#include "stm32373c_eval_tsensor.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup STM32373C_EVAL
  * @{
  */

/** @addtogroup STM32373C_EVAL_TSENSOR
  * @brief      This file includes the LM75 Temperature Sensor driver of 
  *             STM32373C-EVAL boards.
  * @{
  */ 

/** @addtogroup STM32373C_EVAL_TSENSOR_Private_Variables
  * @{
  */ 
static TSENSOR_DrvTypeDef  *tsensor_drv; 
__IO uint16_t  TSENSORAddress = 0;
/**
  * @}
  */ 

/** @addtogroup STM32373C_EVAL_TSENSOR_Private_Functions
  * @{
  */ 

/**
  * @brief  Initializes peripherals used by the I2C Temperature Sensor driver.
  * @retval TSENSOR status
  */
uint32_t BSP_TSENSOR_Init(void)
{ 
  uint8_t ret = TSENSOR_ERROR;
  TSENSOR_InitTypeDef STLM75_InitStructure;

  /* Temperature Sensor Initialization */
  if(Stlm75Drv.IsReady(TSENSOR_I2C_ADDRESS_A01, TSENSOR_MAX_TRIALS) == HAL_OK)
  {
    /* Initialize the temperature sensor driver structure */
    TSENSORAddress = TSENSOR_I2C_ADDRESS_A01;
    tsensor_drv = &Stlm75Drv;

    ret = TSENSOR_OK;
  }
  else
  {
    if(Stlm75Drv.IsReady(TSENSOR_I2C_ADDRESS_A02, TSENSOR_MAX_TRIALS) == HAL_OK)
    {
      /* Initialize the temperature sensor driver structure */
      TSENSORAddress = TSENSOR_I2C_ADDRESS_A02;
      tsensor_drv = &Stlm75Drv;

      ret = TSENSOR_OK;
    }
    else
    {
      ret = TSENSOR_ERROR;
    }
  }

  if (ret == TSENSOR_OK)
  {
    /* Configure Temperature Sensor : Conversion 9 bits in continuous mode */
    /* Alert outside range Limit Temperature 12� <-> 24�c */
    STLM75_InitStructure.AlertMode             = STLM75_INTERRUPT_MODE;
    STLM75_InitStructure.ConversionMode        = STLM75_CONTINUOUS_MODE;
    STLM75_InitStructure.TemperatureLimitHigh  = 24;
    STLM75_InitStructure.TemperatureLimitLow   = 12;
        
    /* TSENSOR Init */   
    tsensor_drv->Init(TSENSORAddress, &STLM75_InitStructure);

    ret = TSENSOR_OK;
  }
  
  return ret;
}

/**
  * @brief  Returns the Temperature Sensor status.
  * @retval The Temperature Sensor status.
  */
uint8_t BSP_TSENSOR_ReadStatus(void)
{
  return (tsensor_drv->ReadStatus(TSENSORAddress));
}

/**
  * @brief  Read Temperature register of LM75.
  * @retval STLM75 measured temperature value.
  */
uint16_t BSP_TSENSOR_ReadTemp(void)
{ 
  return tsensor_drv->ReadTemp(TSENSORAddress);

}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
