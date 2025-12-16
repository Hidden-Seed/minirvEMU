FIXDEP_PATH := $(PROJ_HOME)/tools/fixdep
silent      := -s
FIXDEP      := $(FIXDEP_PATH)/build/fixdep

$(FIXDEP):
	@$(MAKE) $(silent) -C $(FIXDEP_PATH)

define call_fixdep
	@$(FIXDEP) $(1) $(2) unused > $(1).tmp
	@mv $(1).tmp $(1)
endef

clean-fixdep:
	-rm -rf $(FIXDEP_PATH)/build

.PHONY: clean-fixdep
