#include "uc_ports.hpp"
#include "stm32fxxx.h"



#define CANx                           CAN
#define CANx_CLK_ENABLE()              __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()

#define CANx_FORCE_RESET()             __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()           __HAL_RCC_CAN1_RELEASE_RESET()

/* Definition for CANx Pins */
#define CANx_TX_PIN                    GPIO_PIN_1
#define CANx_TX_GPIO_PORT              GPIOD
#define CANx_RX_PIN                    GPIO_PIN_0
#define CANx_RX_GPIO_PORT              GPIOD


/* Definition for CAN's NVIC */
#ifdef STM32_FAMILY_F3
#define CANx_TX_AF                     GPIO_AF7_CAN
#define CANx_RX_AF                     GPIO_AF7_CAN
#define CANx_RX_IRQn                   USB_LP_CAN_RX0_IRQn
#define CANx_RX_IRQHandler             USB_LP_CAN_RX0_IRQHandler
#elif defined STM32_FAMILY_F4
#define CANx_TX_AF                     GPIO_AF9_CAN1
#define CANx_RX_AF                     GPIO_AF9_CAN1
#define CANx_RX_IRQn                   CAN1_RX0_IRQn
#define CANx_RX_IRQHandler             CAN1_RX0_IRQHandler
#endif


namespace
{
    drivers::UcPorts* po_port_configuration = nullptr;
}

namespace drivers
{
    UcPorts::UcPorts()
    {
        ::po_port_configuration = this;
    }
    STM32F303CCT6UcPorts::STM32F303CCT6UcPorts() {}

    int32_t STM32F303CCT6UcPorts::init_ports_can()
    {
        GPIO_InitTypeDef   GPIO_InitStruct;

        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* CAN1 Periph clock enable */
        CANx_CLK_ENABLE();
        /* Enable GPIO clock ****************************************/
        CANx_GPIO_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* CAN1 TX GPIO pin configuration */
        GPIO_InitStruct.Pin = CANx_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Alternate =  CANx_TX_AF;

        HAL_GPIO_Init(CANx_TX_GPIO_PORT, &GPIO_InitStruct);

        /* CAN1 RX GPIO pin configuration */
        GPIO_InitStruct.Pin = CANx_RX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Alternate =  CANx_RX_AF;

        HAL_GPIO_Init(CANx_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC #################################################*/
        /* NVIC configuration for CAN1 Reception complete interrupt */
        HAL_NVIC_SetPriority(CANx_RX_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(CANx_RX_IRQn);
    }

    int32_t STM32F303CCT6UcPorts::deinit_ports_can()
    {
        /*##-1- Reset peripherals ##################################################*/
        CANx_FORCE_RESET();
        CANx_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks ################################*/
        /* De-initialize the CAN1 TX GPIO pin */
        HAL_GPIO_DeInit(CANx_TX_GPIO_PORT, CANx_TX_PIN);
        /* De-initialize the CAN1 RX GPIO pin */
        HAL_GPIO_DeInit(CANx_RX_GPIO_PORT, CANx_RX_PIN);

        /*##-4- Disable the NVIC for CAN reception #################################*/
        HAL_NVIC_DisableIRQ(CANx_RX_IRQn);
    }


    /**
    * @brief TIM_Base MSP Initialization
    * This function configures the hardware resources used in this example
    * @param htim_base: TIM_Base handle pointer
    * @retval None
    */

    int32_t STM32F303CCT6UcPorts::init_ports_tim()
    {
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      //if(htim_base->Instance==TIM2)
      {
      /* USER CODE BEGIN TIM2_MspInit 0 */

      /* USER CODE END TIM2_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM4_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM2 GPIO Configuration
        PA1     ------> TIM2_CH2
        PA2     ------> TIM2_CH3
        */
        /*
        GPIO_InitStruct.Pin = GPIO_PIN_11| GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Alternate = GPIO_AF10_TIM4;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        */

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_TIM4;
        //HAL_GPIO_Init(GPIO_PORT, &GPIO_InitStruct);

        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
/*
        GPIO_InitStruct.Pin = GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Alternate = GPIO_AF10_TIM4;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
*/
        /* TIM2 interrupt Init */
        HAL_NVIC_SetPriority(TIM4_IRQn, 0, 1);
        HAL_NVIC_EnableIRQ(TIM4_IRQn);
      /* USER CODE BEGIN TIM2_MspInit 1 */

      /* USER CODE END TIM2_MspInit 1 */

      }

    }

    /**
    * @brief TIM_Base MSP De-Initialization
    * This function freeze the hardware resources used in this example
    * @param htim_base: TIM_Base handle pointer
    * @retval None
    */
    //void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
    int32_t STM32F303CCT6UcPorts::deinit_ports_tim()
    {
      //if(htim_base->Instance==TIM2)
      {
      /* USER CODE BEGIN TIM2_MspDeInit 0 */

      /* USER CODE END TIM2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM4_CLK_DISABLE();

        /**TIM2 GPIO Configuration
        PA1     ------> TIM2_CH2
        PA2     ------> TIM2_CH3
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

        /* TIM2 interrupt DeInit */
        HAL_NVIC_DisableIRQ(TIM4_IRQn);
      /* USER CODE BEGIN TIM2_MspDeInit 1 */

      /* USER CODE END TIM2_MspDeInit 1 */
      }
    }

    int32_t STM32F303CCT6UcPorts::init_ports_adc()
    {
        return 0;
    }

    int32_t STM32F303CCT6UcPorts::deinit_ports_adc()
    {
        return 0;
    }

    int32_t STM32F303CCT6UcPorts::init_ports_dac()
    {
        return 0;
    }

    int32_t STM32F303CCT6UcPorts::deinit_ports_dac()
    {
        return 0;
    }
}




extern "C"
{

    /**
      * @brief CAN MSP Initialization
      *        This function configures the hardware resources used in this example:
      *           - Peripheral's clock enable
      *           - Peripheral's GPIO Configuration
      *           - NVIC configuration for DMA interrupt request enable
      * @param hcan: CAN handle pointer
      * @retval None
      */
    void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
    {
        if (nullptr != po_port_configuration)
        {
            po_port_configuration->init_ports_can();
        }
    }

    /**
      * @brief CAN MSP De-Initialization
      *        This function frees the hardware resources used in this example:
      *          - Disable the Peripheral's clock
      *          - Revert GPIO to their default state
      * @param hcan: CAN handle pointer
      * @retval None
      */
    void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan)
    {
        if (nullptr != po_port_configuration)
        {
            po_port_configuration->deinit_ports_can();
        }
    }

    void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
    {
        if (nullptr != po_port_configuration && htim_base->Instance==TIM4)
        {
            po_port_configuration->init_ports_tim();
        }
    }

    void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
    {
        if (nullptr != po_port_configuration && htim_base->Instance==TIM4)
        {
            po_port_configuration->deinit_ports_tim();
        }
    }



}
