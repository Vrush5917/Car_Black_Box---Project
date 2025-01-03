
#include "main.h"

unsigned char eeprom_at24c04_random_read(unsigned char addr)
{
    unsigned char data = 0;
    
    i2c_start();
    i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ_EEPROM);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void eeprom_at24c04_byte_write(unsigned char addr, unsigned char data)
{
    i2c_start();
    i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
    __delay_ms(3);
}
void eeprom_at24c04_str_write(unsigned char addr, unsigned char *data)
{
    
    while(*data != '\0')
    {
        eeprom_at24c04_byte_write(addr, *data);
        data++;
        addr++;
    }

}

