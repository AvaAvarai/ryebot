# This Makefile assumes the top folder has been built
TOP = ../
CC ?= gcc

COGUTILS_DIR  = $(TOP)/cog-utils
CORE_DIR      = $(TOP)/core
INCLUDE_DIR   = $(TOP)/include
GENCODECS_DIR = $(TOP)/gencodecs

BOTS       = bot

CFLAGS  = -I$(INCLUDE_DIR) -I$(COGUTILS_DIR) -I$(CORE_DIR) \
          -I$(CORE_DIR)/third-party -I$(GENCODECS_DIR)     \
          -O0 -g -pthread -Wall
LDFLAGS = -L$(TOP)/lib
LDLIBS  = -ldiscord -lcurl

all: $(BOTS)

echo:
	@ echo -e 'CC: $(CC)\n'
	@ echo -e 'BOTS: $(BOTS)\n'

clean:
	@ $(RM) $(BOTS) $(VOICE_BOTS)

.PHONY: all echo clean
