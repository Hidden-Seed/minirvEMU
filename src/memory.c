#include <common.h>
#include <isa.h>
#include <memory-emu.h>

static uint8_t emu_memory[MEMORY_SIZE] = {};

uint8_t *guest_to_host(addr_t addr) {
  return emu_memory + addr - MEMORY_BASE;
}
addr_t host_to_guest(uint8_t *addr) {
  return addr - emu_memory + MEMORY_BASE;
}

static inline word_t host_read(void *addr, int len) {
  switch (len) {
    case 1:
#ifdef MTRACE_ENABLE
      printf("[READ] M[" FMT_ADDR "] = 0x%02x (8-bit)\n", host_to_guest(addr), *(uint8_t *)addr);
#endif
      return *(uint8_t *)addr;
    case 2:
#ifdef MTRACE_ENABLE
      printf("[READ] M[" FMT_ADDR "] = 0x%04x (16-bit)\n", host_to_guest(addr), *(uint16_t *)addr);
#endif
      return *(uint16_t *)addr;
    case 4:
#ifdef MTRACE_ENABLE
      printf("[READ] M[" FMT_ADDR "] = 0x%08x (32-bit)\n", host_to_guest(addr), *(uint32_t *)addr);
#endif
      return *(uint32_t *)addr;
    default:
      assert(0);
      return 0;
  }
}

static inline void host_write(void *addr, int len, word_t data) {
  switch (len) {
    case 1:
      *(uint8_t *)addr = data;
#ifdef MTRACE_ENABLE
      printf("[WRITE] M[" FMT_ADDR "] = 0x%02x (8-bit)\n", host_to_guest(addr), *(uint8_t *)addr);
#endif
      return;
    case 2:
      *(uint16_t *)addr = data;
#ifdef MTRACE_ENABLE
      printf("[WRITE] M[" FMT_ADDR "] = 0x%04x (16-bit)\n", host_to_guest(addr), *(uint16_t *)addr);
#endif
      return;
    case 4:
      *(uint32_t *)addr = data;
#ifdef MTRACE_ENABLE
      printf("[WRITE] M[" FMT_ADDR "] = 0x%08x (32-bit)\n", host_to_guest(addr), *(uint32_t *)addr);
#endif
      return;
    default:
      assert(0);
  }
}

static word_t mem_read(addr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void mem_write(addr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

static void out_of_bound(addr_t addr) {
  panic("address = " FMT_ADDR " is out of bound of pmem [" FMT_ADDR ", " FMT_ADDR "] at pc = " FMT_WORD,
        addr, MEM_LEFT, MEM_RIGHT, cpu.pc);
}

void init_mem() {
  memset(emu_memory, rand(), MEMORY_SIZE);
  Log("physical memory area [" FMT_ADDR ", " FMT_ADDR "]", MEM_LEFT, MEM_RIGHT);
}

static inline bool in_mem(addr_t addr) {
  return addr - MEMORY_BASE < MEMORY_SIZE;
}

word_t addr_read(addr_t addr, int len) {
  if (likely(in_mem(addr))) return mem_read(addr, len);
  out_of_bound(addr);
  return 0;
}

bool in_vga_buf(addr_t addr);
void out_of_vga_buf(addr_t addr);
void vga_buf_write(addr_t addr, word_t data);

void addr_write(addr_t addr, int len, word_t data) {
  if (likely(in_mem(addr))) {
    mem_write(addr, len, data);
    return;
  } else if (in_vga_buf(addr)) {
    vga_buf_write(addr, data);
    return;
  }
  out_of_bound(addr);
  out_of_vga_buf(addr);
}
