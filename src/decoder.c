#include <common.h>
#include <isa.h>
#include <memory-emu.h>

#define inst_len (32 + 6)

// --- pattern matching mechanism ---
__attribute__((always_inline)) static inline void pattern_decode(
    const char *str, uint32_t *key, uint32_t *mask, uint32_t *shift) {
  uint32_t __key = 0, __mask = 0, __shift = 0;

  for (int i = 0; i < inst_len; i++) {
    char c = str[i];
    if (c != ' ') {
      Assert(c == '0' || c == '1' || c == '?', "invalid character '%c' in pattern string", c);
      __key   = (__key << 1) | (c == '1' ? 1 : 0);
      __mask  = (__mask << 1) | (c == '?' ? 0 : 1);
      __shift = (c == '?' ? __shift + 1 : 0);
    }
  }

  *key   = __key >> __shift;
  *mask  = __mask >> __shift;
  *shift = __shift;
}

// --- pattern matching wrappers for decode ---
#define INSTPAT(pattern, ...)                     \
  do {                                            \
    uint32_t key, mask, shift;                    \
    pattern_decode(pattern, &key, &mask, &shift); \
    if (((inst >> shift) & mask) == key) {        \
      INSTPAT_MATCH(inst, ##__VA_ARGS__);         \
    }                                             \
  } while (0)

// clang-format off
enum {TYPE_U, TYPE_I, TYPE_S, TYPE_R, TYPE_N };
// clang-format on

#define REG(rd) cpu.gpr[rd]
// clang-format off
#define src1R() do { *src1 = REG(rs1); } while (0)
#define src2R() do { *src2 = REG(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
// clang-format on

#define MR addr_read
#define MW addr_write

static void decode_operand(word_t inst, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  word_t i = inst;
  int rs1  = BITS(i, 19, 15);
  int rs2  = BITS(i, 24, 20);
  *rd      = BITS(i, 11, 7);
  // clang-format off
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_N:                           break;
    default: panic("unsupported type = %d", type);
  }
  // clang-format on
}

void cpu_trap(word_t halt_pc, int halt_ret);
#define TRAP cpu_trap

void decode_exec(word_t inst, addr_t pc, addr_t *npc) {
#define INSTPAT_MATCH(inst, name, type, ... /* execute body */)         \
  {                                                                     \
    int rd      = 0;                                                    \
    word_t src1 = 0, src2 = 0, imm = 0;                                 \
    decode_operand(inst, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
    __VA_ARGS__;                                                        \
    goto finish;                                                        \
  }

  bool npc_jump = false;

  // clang-format off
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, REG(rd) = imm);
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, REG(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, REG(rd) = SEXT(MR(src1 + imm, 4), 32));
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, REG(rd) = MR(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, REG(rd) = pc + 4; *npc = (src1 + imm) & (~1); npc_jump = true); // Set the least-significant bit of the result to zero.
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, MW(src1 + imm, 4, src2));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, MW(src1 + imm, 1, src2));
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, REG(rd) = src1 + src2);
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, TRAP(pc, REG(10))); // REG(10) is $a0
  // clang-format on

finish:
  REG(0) = 0;  // reset $zero to 0
  if (!npc_jump) {
    *npc = pc + 4;
  }
}
