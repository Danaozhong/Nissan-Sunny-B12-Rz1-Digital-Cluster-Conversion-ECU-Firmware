#ifndef SRC_ADC_INC_STM32_ADC_HPP_
#define SRC_ADC_INC_STM32_ADC_HPP_

#include "generic_adc.hpp"
#include "hwheader.h"

#include <optional>

namespace drivers
{
    class STM32ADC : public GenericADC
    {
    public:
        /** Parameter constructor */
        STM32ADC(ADCResolution en_resolution, ADC_TypeDef* pt_adc_peripheral, uint32_t u32_adc_channel, GPIO_TypeDef* pt_gpio_block, uint16_t u16_gpio_pin);

        /** Destructor. */
        virtual ~STM32ADC();

        /**
        \brief returns the minimum value the ADC can possibly read.
        */
        virtual uint32_t get_adc_min_value() const;
        
        /**
        \brief returns the maximum value the ADC can possibly read.
        */
        virtual uint32_t get_adc_max_value() const;

        /**
        \brief Triggers an ADC conversion, and returns the read ADC value.
        */
        virtual std::optional<uint32_t> read_adc_value();
    private:
#ifdef HAL_ADC_MODULE_ENABLED
        /// The handle to the ADC object.
        ADC_HandleTypeDef m_adc_handle;
#endif
        /// \brief Specifies the configured ADC resolution.
        ADCResolution m_en_adc_resolution;
        
        /// \brief The ADC channel of the Microprocessor used.
        uint32_t m_u32_adc_channel;

        /** The GPIO where this ADC is connected to */
        GPIO_TypeDef* m_pt_gpio_block;

        /** The pin of the GPIO block m_pt_gpio_block where the ADC is connected to */
        uint16_t m_u16_gpio_pin;
        
        /// The ADC peripheral used.
        ADC_TypeDef* m_pt_adc_peripheral;
    };
}

#endif /* SRC_ADC_INC_STM32_ADC_HPP_ */
