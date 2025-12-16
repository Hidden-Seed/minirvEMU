SRCS += $(shell find -L src -name "*.c")

LIBCAPSTONE = tools/capstone/repo/libcapstone.so.5
CFLAGS += -I tools/capstone/repo/include
src/disasm.c: $(LIBCAPSTONE)
$(LIBCAPSTONE):
	$(MAKE) -C tools/capstone
