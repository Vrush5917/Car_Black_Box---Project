/* 
 * File:   External_EEPROM.h
 * Author: magdu
 *
 * Created on 15 November, 2024, 11:57 AM
 */

#ifndef EXTERNAL_EEPROM_H
#define	EXTERNAL_EEPROM_H

#define SLAVE_WRITE_EEPROM            0b10100000 // 0xD0
#define SLAVE_READ_EEPROM             0b10100001 // 0xD1



unsigned char eeprom_at24c04_random_read(unsigned char addr);
void eeprom_at24c04_str_write(unsigned char addr, unsigned char *data);

#endif	/* EXTERNAL_EEPROM_H */

