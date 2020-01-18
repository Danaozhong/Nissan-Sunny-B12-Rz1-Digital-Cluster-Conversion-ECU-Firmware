#ifndef _STM32_FXXX_IT_H_
#define _STM32_FXXX_IT_H_


#if defined(STM32_FAMILY_F3)
#include "stm32f3xx_it.h"
#elif defined(STM32_FAMILY_F4)
#include "stm32f3xx_it.h"
#else
#error "No interrupt header file found (unknown controller)!"
#endif

#elif STM32_FAMILY
#endif /* _STM32_FXXX_IT_H_ */
