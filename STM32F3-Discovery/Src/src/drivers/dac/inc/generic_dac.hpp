/*
 * generic_dac.hpp
 *
 *  Created on: 27.09.2019
 *      Author: Clemens
 */

#ifndef SRC_DAC_GENERIC_DAC_HPP_
#define SRC_DAC_GENERIC_DAC_HPP_

#include <cstdint>

namespace drivers
{
	/** Generic base class to handle the functionality of an DAC. Can be a STM32 internal DAC, or
	 * an DAC controlled by a PWM, or whatever...
	 */
	class GenericDAC
	{
	public:
		GenericDAC();
		virtual ~GenericDAC();

		/** Function to set the output by value */
		virtual int32_t set_output_value(uint32_t value) = 0;
		virtual uint32_t get_max_value() const = 0;
		virtual uint32_t get_min_value() const = 0;

		virtual int32_t set_output_voltage(float value) = 0;
		virtual float get_max_voltage() const = 0;
		virtual float get_min_voltage() const = 0;
	};
}

#endif /* SRC_DAC_GENERIC_DAC_HPP_ */
