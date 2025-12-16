#include <common.h>
#include <isa.h>
#include <memory-emu.h>

// clang-format off
const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
// clang-format on

int reg_name_to_index(const char *name) {
  for (int i = 0; i < NR_GPR; i++) {
    if (strcmp(regs[i], name) == 0)
      return i;
  }
  panic("invalid GPR index");
}

void isa_reg_display() {
  int idx, count = 0;

  for (idx = 0; idx < NR_GPR; idx++) {
    printf("%s\t" FMT_WORD "\t", regs[idx], cpu.gpr[idx]);

    count++;
    if (count == 2 || idx == NR_GPR - 1) {
      printf("\n");
      count = 0;
    }
  }
}

void init_isa() {
  // Set the initial program counter.
  cpu.pc = RESET_VECTOR;
  // The zero register is always 0.
  gpr("$0") = 0;
}

void restart() {
  init_isa();
}

/*
  Instructions supported
  add, addi, lui, lw, lbu, sw, sb, jalr, ebreak
  - U-type: lui
  - I-type: addi, lw, lbu, jalr
  - S-type: sw, sb
  - R-type: add
  - N-type(none): ebreak
*/

void decode_exec(word_t inst, addr_t pc, addr_t *npc);

void isa_exec_once(CPU_state *cpu) {
  decode_exec(cpu->inst, cpu->pc, &cpu->npc);
}
