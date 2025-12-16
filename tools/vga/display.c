#include <am.h>
#include <klib-macros.h>
#include <stdio.h>

#define VGA_WIDTH 256
#define VGA_HEIGHT 256
#define VGA_BUF_SIZE 0x40000
static uint8_t vga_buf[VGA_BUF_SIZE];

void load_vga_buf(const char *filename) {
  FILE *fp = fopen(filename, "rb");  // r = read, b = binary
  if (!fp) {
    perror("fopen");
    return;
  }

  size_t n = fread(vga_buf, 1, VGA_BUF_SIZE, fp);
  if (n != VGA_BUF_SIZE) {
    fprintf(stderr, "Warning: read %zu bytes, expect %d\n",
            n, VGA_BUF_SIZE);
  }

  fclose(fp);
}

int main() {
  ioe_init();  // initialization for GUI

  load_vga_buf("../../build/display.bin");
  io_write(AM_GPU_FBDRAW, 0, 0, (uint32_t *)vga_buf, VGA_WIDTH, VGA_HEIGHT, true);

  putstr("Press any key to quit...\n");
  while (1) {
    AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
    if (ev.keycode != AM_KEY_NONE) break;
  }

  return 0;
}