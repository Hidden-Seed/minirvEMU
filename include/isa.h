#ifndef __ISA_H__
#define __ISA_H__

#define NR_GPR 32
#define DECODE_LOGBUF_SIZE 128
typedef struct {
  addr_t pc;
  word_t gpr[NR_GPR];
  word_t inst;
  addr_t npc;
  char logbuf[DECODE_LOGBUF_SIZE];
} CPU_state;

extern CPU_state cpu;

void init_isa();
void isa_exec_once(CPU_state *cpu);

int reg_name_to_index(const char *name);
#define gpr(name) cpu.gpr[reg_name_to_index(name)]
void isa_reg_display();

#endif