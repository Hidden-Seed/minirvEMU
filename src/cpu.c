#include <common.h>
#include <isa.h>
#include <memory-emu.h>

CPU_state cpu;

typedef struct {
  bool cpu_running;
  word_t halt_pc;
  int halt_ret;  // ret = 0 --> good trap
} emu_state;

emu_state state = {
    .cpu_running = true,
    .halt_pc     = 0,
    .halt_ret    = 1};

bool vga_enable = false;

word_t inst_fetch(addr_t pc, int len) {
  return addr_read(pc, len);
}

void cpu_trap(word_t halt_pc, int halt_ret) {
  state.cpu_running = false;
  state.halt_pc     = halt_pc;
  state.halt_ret    = halt_ret;
}

#ifdef ITRACE_ENABLE
static void write_cpu_log() {
  char *p = cpu.logbuf;
  p += snprintf(p, sizeof(cpu.logbuf), FMT_WORD ":", cpu.pc);
  int ilen      = 4;
  uint8_t *inst = (uint8_t *)&cpu.inst;
  for (int i = ilen - 1; i >= 0; i--) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }

  int space_len = 4;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, cpu.logbuf + sizeof(cpu.logbuf) - p, cpu.pc, (uint8_t *)&cpu.inst, ilen);

  puts(cpu.logbuf);
}
#endif

void assert_fail_msg() {
  isa_reg_display();
}

int flag = 0;

void cpu_exec(uint64_t n) {
  while (n--) {
    cpu.inst = inst_fetch(cpu.pc, 4);
    isa_exec_once(&cpu);
#ifdef ITRACE_ENABLE
    write_cpu_log();
#endif
    cpu.pc = cpu.npc;

    if (!state.cpu_running) {
      Log("minirvEMU: %s " ANSI_FG_BLUE "at pc = " FMT_WORD,
          ((state.halt_ret == 0) ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) : ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED)),
          state.halt_pc);
      void vga_display(bool vga_enable);
      vga_display(vga_enable);
      break;
    }
  }
}