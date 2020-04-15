###Setup taken from subdirectory in pin-3.13.98189-g60a6ef199-gcc-linux/source/tools/SimpleExamples##


ifdef PIN_ROOT
CONFIG_ROOT := $(PIN_ROOT)/source/tools/Config
else
CONFIG_ROOT := ../Config
endif
include $(CONFIG_ROOT)/makefile.config
include makefile.rules
include $(TOOLS_ROOT)/Config/makefile.default.rules
