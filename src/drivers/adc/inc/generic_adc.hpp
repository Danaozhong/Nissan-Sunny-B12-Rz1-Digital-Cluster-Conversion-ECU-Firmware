/*
 * generic_adc.hpp
 *
 *  Created on: 28.09.2019
 *      Author: Clemens
 */

#ifndef SRC_ADC_INC_GENERIC_ADC_HPP_
#define SRC_ADC_INC_GENERIC_ADC_HPP_

#include <optional>
#include <cstdint>

namespace drivers
{
    enum ADCResolution
    {
        ADC_RESOLUTION_8BIT,
        ADC_RESOLUTION_10BIT,
        ADC_RESOLUTION_12BIT
    };

    class GenericADC
    {
    public:
        /** Destructor */
        virtual ~GenericADC() {}
        virtual uint32_t get_adc_min_value() const = 0;
        virtual uint32_t get_adc_max_value() const = 0;

        virtual std::optional<uint32_t> read_adc_value() = 0;
    };
}



#endif /* SRC_ADC_INC_GENERIC_ADC_HPP_ */
