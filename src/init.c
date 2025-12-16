#include <memory-emu.h>

static char *log_file     = NULL;
static char *img_file     = NULL;
static uint32_t halt_base = 0;
extern bool vga_enable;

static void welcome() {
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to minirvEMU!\n");
}

void init_log(const char *log_file);

static void add_ebreak() {
  uint32_t ebreak    = 0x00100073;
  uint8_t *mem       = (uint8_t *)guest_to_host(RESET_VECTOR + halt_base);
  *(uint32_t *)(mem) = ebreak;
  Log("Set ebreak at: " FMT_ADDR ", value: " FMT_WORD, RESET_VECTOR + halt_base, *(uint32_t *)mem);
}

static long load_img() {
  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  add_ebreak();

  return size;
}

#include <getopt.h>

static int parse_args(int argc, char *argv[]) {
  // clang-format off
	const struct option table[] = {
    {"log"       , required_argument, NULL, 'l'},
    {"help"      , no_argument      , NULL, 'h'},
    {"halt_base" , required_argument, NULL, 'B'},
    {"vga_enable", no_argument      , NULL, 'd'},
    {0           , 0                , NULL,  0 }, 
  };
  // clang-format on

  int o;
  while ((o = getopt_long(argc, argv, "-hl:B:d", table, NULL)) != -1) {
    switch (o) {
      case 'l':
        log_file = optarg;
        break;
      case 'B':
        halt_base = (unsigned int)strtoul(optarg, NULL, 0);
        break;
      case 'd':
        vga_enable = true;
        break;
      case 1:
        img_file = optarg;
        break;
      case 'h':
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-l,--log=FILE         output log to FILE\n");
        printf("\t-d,--vga_enable       enable vga display\n");
        printf("\t-B,--halt_base=addr   set ebreak at addr\n");
        printf("\t-h,--help             print usage help  \n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_isa();
void init_disasm();
void init_vga();

void init_minirvEMU(int argc, char *argv[]) {
  parse_args(argc, argv);
  init_log(log_file);
  init_mem();
  init_vga();
  init_isa();
  load_img();
  init_disasm();
  welcome();
}