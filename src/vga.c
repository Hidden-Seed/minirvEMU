#include <common.h>
#include <isa.h>

#define VGA_BUF_BASE 0x20000000
#define VGA_BUF_SIZE 0x40000
#define VGA_BUF_LEFT ((addr_t)VGA_BUF_BASE)
#define VGA_BUF_RIGHT ((addr_t)(VGA_BUF_LEFT + VGA_BUF_SIZE - 1))

#define VGA_WIDTH 256
#define VGA_HEIGHT 256

static uint8_t vga_buf[VGA_BUF_SIZE] = {};

void init_vga() {
  memset(vga_buf, rand(), VGA_BUF_SIZE);
  Log("physical memory area [" FMT_ADDR ", " FMT_ADDR "]", VGA_BUF_LEFT, VGA_BUF_RIGHT);
}

static uint8_t *guest_to_host(addr_t addr) {
  return vga_buf + addr - VGA_BUF_BASE;
}

bool in_vga_buf(addr_t addr) {
  return addr - VGA_BUF_BASE < VGA_BUF_SIZE;
}

void out_of_vga_buf(addr_t addr) {
  panic("address = " FMT_ADDR " is out of bound of vga buffer [" FMT_ADDR ", " FMT_ADDR "] at pc = " FMT_WORD,
        addr, VGA_BUF_LEFT, VGA_BUF_RIGHT, cpu.pc);
}

// len = 4
void vga_buf_write(addr_t addr, word_t data) {
  *(uint32_t *)(guest_to_host(addr)) = data;
  return;
}

void vga_display(bool vga_enable) {
  if (!vga_enable) return;

  FILE *fp = fopen("build/display.bin", "wb");  // b = binary
  if (!fp) {
    perror("fopen");
    return;
  }

  fwrite(vga_buf, sizeof(uint8_t), VGA_BUF_SIZE, fp);
  fclose(fp);

  int ret = system("make -C tools/vga ARCH=native run");
  if (ret != 0) {
    panic("vga dispaly error");
  }
}
