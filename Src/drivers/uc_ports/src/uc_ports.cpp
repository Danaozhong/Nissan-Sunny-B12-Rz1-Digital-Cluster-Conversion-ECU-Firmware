#include "uc_ports.hpp"
#include "stm32f3xx.h"



#define CANx                           CAN
#define CANx_CLK_ENABLE()              __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()

#define CANx_FORCE_RESET()             __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()           __HAL_RCC_CAN1_RELEASE_RESET()

/* Definition for CANx Pins */
#define CANx_TX_PIN                    GPIO_PIN_1
#define CANx_TX_GPIO_PORT              GPIOD
#define CANx_TX_AF                     GPIO_AF7_CAN
#define CANx_RX_PIN                    GPIO_PIN_0
#define CANx_RX_GPIO_PORT              GPIOD
#define CANx_RX_AF                     GPIO_AF7_CAN

/* Definition for CAN's NVIC */
#define CANx_RX_IRQn                   USB_LP_CAN_RX0_IRQn
#define CANx_RX_IRQHandler             USB_LP_CAN_RX0_IRQHandler


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

}
