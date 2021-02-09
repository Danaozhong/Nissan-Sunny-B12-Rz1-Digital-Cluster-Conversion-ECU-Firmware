#ifndef __EEPROMCONFIG_H
#define __EEPROMCONFIG_H

// This switch can be used to automatically erase invalidated data, but needs more RAM
#define   _EEPROM_AUTO_ERASE___NEED_MORE_RAM                        (0)


// Which board we are using
#define   _EEPROM_F030x4_F030x6_F070x6_F030x8                       (0)

#define   _EEPROM_F303xC_F303xB                                     (1)

#define   _EEPROM_F1_LOW_DESTINY                                    (0)
#define   _EEPROM_F1_MEDIUM_DESTINY                                 (0)
#define   _EEPROM_F1_HIGH_DESTINY                                   (0)

// use the last flash page for EEPROM emulation
#define   _EEPROM_USE_FLASH_PAGE                                    (255)

#endif
