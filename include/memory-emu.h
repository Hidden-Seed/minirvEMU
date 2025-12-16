#ifndef __MEMORY_EMU_H__
#define __MEMORY_EMU_H__
// avoid conflicts with memory.h
#include <common.h>

#define NUM_KILO 1024
#define NUM_MEGA (NUM_KILO * NUM_KILO)
// 1MB size RAM
#define MEMORY_SIZE (int)(1 * NUM_MEGA)
#define MEMORY_BASE 0x00000000
#define MEM_LEFT ((addr_t)MEMORY_BASE)
#define MEM_RIGHT ((addr_t)(MEM_LEFT + MEMORY_SIZE - 1))
#define RESET_VECTOR MEM_LEFT

uint8_t *guest_to_host(addr_t addr);
addr_t host_to_guest(uint8_t *addr);

void init_mem();

word_t addr_read(addr_t addr, int len);
void addr_write(addr_t addr, int len, word_t data);

#endif