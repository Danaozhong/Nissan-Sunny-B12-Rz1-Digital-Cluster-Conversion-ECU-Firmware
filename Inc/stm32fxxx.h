#ifndef _STM32_FXXXX_H_
#define _STM32_FXXXX_H_


#if defined(STM32_FAMILY_F3)
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#elif defined(STM32_FAMILY_F4)
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#endif

#elif STM32_FAMILY
#endif /* _STM32_FXXXX_H_ */
