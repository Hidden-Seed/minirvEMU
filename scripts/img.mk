IMG_PATH = $(PROJ_HOME)/test

# test_file list:
# mem sum vga
IMG_FILE ?= mem
IMG_FILE_HEX = $(IMG_PATH)/$(IMG_FILE).hex
IMG_FILE_BIN = $(BUILD_DIR)/$(IMG_FILE).bin

$(IMG_FILE_BIN): $(IMG_FILE_HEX)
	@echo "Using HEX file: $<"
	@tail -n +2 $^ | sed -e 's/.*: //' -e 's/ /\n/g' | \
		sed -e 's/\(..\)\(..\)\(..\)\(..\)/\4 \3 \2 \1/' | xxd -r -p > $@

# find <halt>
IMG_FILE_TXT = $(IMG_PATH)/$(IMG_FILE).txt
HALT_BASE   := $(shell grep '<halt>:' $(IMG_FILE_TXT) | sed -n 's/^\([0-9a-fA-F]\+\) <halt>:.*/\1/p')

VGA_DISPLAY = $(PROJ_HOME)/tools/vga
