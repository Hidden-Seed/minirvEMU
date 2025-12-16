EMU_NAME  = minirvEMU
PROJ_HOME = $(shell pwd)

# Include all filelist.mk to merge file lists
FILELIST_MK = $(shell find -L $(PROJ_HOME) -name "filelist.mk")
include $(FILELIST_MK)

# Include rules for fixdep
include $(PROJ_HOME)/scripts/fixdep.mk

# Include rules to build minirvEMU
include $(PROJ_HOME)/scripts/build.mk
HALT_BASE ?= 0
CFLAGS += -DHALT_BASE=0x$(HALT_BASE) 
build: $(FIXDEP) $(BINARY)

# include rules to generate BINARY
include $(PROJ_HOME)/scripts/img.mk

LOG_FILE ?= $(EMU_NAME)-log.txt
ARGS ?= --log=$(BUILD_DIR)/$(LOG_FILE) --halt_base=0x$(HALT_BASE)
ifeq ($(IMG_FILE),vga)
ARGS += --vga_enable
endif

run: build $(IMG_FILE_BIN)
	$(BINARY) $(ARGS) $(IMG_FILE_BIN)

gdb:
	gdb -s $(BINARY) --args $(EMU_EXEC)

clean-all: clean clean-fixdep
	@$(MAKE) -C $(VGA_DISPLAY) clean

.PHONY: build run gdb clean-all
