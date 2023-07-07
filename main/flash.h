#ifndef FLASH_H
#define FLASH_H

void flash_init(void);

// void write_flash_16(uint16_t value);
// uint16_t read_flash_16(void);

void flash_position_write(uint32_t value);
void flash_haight_write(uint32_t value);

uint32_t flash_position_read(void);
uint32_t flash_haight_read(void);

void flash_enocean_write(uint32_t value);
uint32_t flash_enocean_read(void);

#endif