# minirvEMU

`minirvEMU` is a **minimal RISC-V (RV32) emulator** designed for educational purposes.
It focuses on a very small and well-defined subset of the RV32 instruction set, making it suitable for learning **instruction execution, memory access, and basic CPU control flow**.

## Features

### Architecture

- **ISA**: RISC-V RV32 (subset)
- **Register File**:
  - 32 General-Purpose Registers (`x0`–`x31`)
  - Each register is **32 bits**
  - `x0` is hard-wired to zero
- **Program Counter**:
  - 32-bit PC
  - Initial PC value: **0x00000000**

### Supported Instructions

`minirvEMU` currently supports **9 core instructions**:

| Category          | Instructions  |
| ----------------- | ------------- |
| Arithmetic        | `add`, `addi` |
| Immediate / Upper | `lui`         |
| Load              | `lw`, `lbu`   |
| Store             | `sw`, `sb`    |
| Control Flow      | `jalr`        |
| System            | `ebreak`      |

### Test Programs

Three test programs are provided to validate different subsystems of the emulator:

- **`mem`** – Memory access and load/store behavior
- **`sum`** – Arithmetic and register operations
- **`vga`** – Framebuffer and VGA output via AM GPU interface

The test binaries can be found here:

🔗 https://ysyx.oscc.cc/slides/resources/archive/logisim-bin.tar.bz2

## Project Structure

```bash
minirvEMU
├── include
│   ├── common.h                   # Common definitions and global includes
│   ├── isa.h                      # ISA-related definitions (RISC-V RV32)
│   ├── macro.h                    # Common macros and utility macros
│   ├── memory-emu.h               # Memory emulation interface and helpers
│   └── utils.h                    # Miscellaneous utility functions
├── scripts
│   ├── build.mk                   # Build rules and compiler flags
│   ├── fixdep.mk                  # Dependency generation and fixing rules
│   └── img.mk                     # Image selection and IMG_FILE handling
├── src
│   ├── cpu.c                      # CPU execution loop and top-level control
│   ├── decoder.c                  # Instruction decode logic
│   ├── disasm.c                   # Instruction disassembler (debugging)
│   ├── filelist.mk                # Source file list for the build system
│   ├── init.c                     # Emulator initialization routines
│   ├── isa.c                      # Instruction semantics implementation
│   ├── log.c                      # Log support
│   ├── memory.c                   # Memory system emulation
│   ├── minirvEMU-main.c           # Program entry point (main function)
│   └── vga.c                      # VGA support
├── test                           # Test programs: sum, mem and vga
├── tools
│   ├── capstone                   # Capstone-based disassembly support
│   ├── fixdep                     # Dependency fixing tool
│   └── vga                        # Native VGA viewer based on abstract-machine
├── .clang-format
├── .gitignore
├── Makefile
└── README.md
```

## Build and Run

### Requirements

- GNU Make

- GCC / Clang (host build)

- [Abstract Machine (AM) environment](https://github.com/NJU-ProjectN/abstract-machine): environment variable `AM_HOME` must be set

  ```bash
  export AM_HOME=/path/to/abstract-machine
  ```

### Running the Emulator

The emulator is executed via `make run`, with the test program selected using `IMG_FILE`.

```bash
# Run memory access test
make run IMG_FILE=mem

# Run arithmetic sum test
make run IMG_FILE=sum

# Run VGA framebuffer test
make run IMG_FILE=vga
```

Each test program is loaded as an image and executed starting from **PC = 0**.

## VGA Support 

When `IMG_FILE=vga` is specified:

- The emulator enables VGA framebuffer support
- Framebuffer data is written through the AM GPU interface
- A native VGA viewer can be launched to visualize the output ([the logo of ysyx]([ysyx.png (582×535)](https://ysyx.oscc.cc/res/images/logo/ysyx.png)))

> [!NOTE]
>
> For more information, see [E4 从C代码到二进制程序 | 官方文档](https://ysyx.oscc.cc/docs/2407/e/4.html#minirvemu的实现).
>
> Reference Project: [NJU-ProjectN/nemu: NJU EMUlator, a full system x86/mips32/riscv32/riscv64 emulator for teaching](https://github.com/NJU-ProjectN/nemu)