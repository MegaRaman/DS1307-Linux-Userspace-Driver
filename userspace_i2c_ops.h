#ifndef I2C_OPS_DS1307
#define I2C_OPS_DS1307

#include <stdint.h>

int i2c_init(int adapter_nr, int addr);

int i2c_print_func(void);

int i2c_read(uint8_t reg, uint8_t *buf, uint8_t count);

int i2c_write(uint8_t reg, uint8_t *buf, uint8_t count);

#endif
