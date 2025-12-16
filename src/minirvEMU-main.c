#include <common.h>

void init_minirvEMU(int argc, char *argv[]);
void cpu_exec(uint64_t n);

int main(int argc, char **argv) {
  init_minirvEMU(argc, argv);
  cpu_exec(-1);
  return 0;
}