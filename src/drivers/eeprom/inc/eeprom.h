#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

//################################################################################################################
bool EE_ErasePage(uint16_t page);

bool 			EE_Read(uint16_t VirtualAddress, uint32_t* Data);
bool 			EE_Write(uint16_t VirtualAddress, uint32_t Data);
bool			EE_Reads(uint16_t StartVirtualAddress,uint16_t HowMuchToRead,uint32_t* Data);
uint32_t EE_Writes(uint16_t StartVirtualAddress,uint16_t HowMuchToWrite,uint32_t* Data);
uint16_t	EE_GetSize(void);
//################################################################################################################

#ifdef __cplusplus
}
#endif

#endif
