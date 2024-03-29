
CC = clang-12
RE2C = re2c

EXE_NAME = bc

LOG_FLAGS = -DLOG_USE_COLOR -DLOG_LEVEL=4

BUILDDIR = build
GENERATED_SOURCES_DIR = $(BUILDDIR)/generated_sources
GENERATED_INCLUDES_DIR = $(BUILDDIR)/generated_includes
OBJECT_FILE_DIR = $(BUILDDIR)/objs/c
BIN_DIR = $(BUILDDIR)/bin
EXE_FILE = $(BIN_DIR)/$(EXE_NAME)

SRC_DIR = src
CSRC_DIR = $(SRC_DIR)/c
INCLUDE_DIR = $(SRC_DIR)/include

SRC_SUBDIRS = $(shell find $(CSRC_DIR)/* -type d)
OBJ_SUBDIRS = $(patsubst $(CSRC_DIR)/%,$(OBJECT_FILE_DIR)/%,$(SRC_SUBDIRS))


RE2_SOURCES = $(wildcard $(SRC_DIR)/re2c/*.re)
GENERATED_SOURCES = $(patsubst $(SRC_DIR)/re2c/%.re,$(GENERATED_SOURCES_DIR)/%.re.c,$(RE2_SOURCES))

LOG_C_SOURCE_DIR = ../external/log.c/src
LOG_C_INCLUDES = ../external/log.c/src
LOG_C_OBJS_DIR = $(BUILDDIR)/objs/ext/logc
LOG_C_OBJ = $(LOG_C_OBJS_DIR)/log.o

C_SOURCES = $(shell find $(CSRC_DIR) -type f -name '*.c')
OBJECTS = $(patsubst $(CSRC_DIR)/%.c,$(OBJECT_FILE_DIR)/%.o,$(C_SOURCES))
OBJECTS += $(patsubst $(GENERATED_SOURCES_DIR)/%.c,$(OBJECT_FILE_DIR)/%.o,$(GENERATED_SOURCES))
OBJECTS += $(LOG_C_OBJ)
DEPS = $(OBJECTS:.o=.d)


RE2C_FLAGS = -W -Werror -i --utf8 --case-ranges

ERR_FLAGS = -Werror=incompatible-pointer-types -Werror=int-conversion -Werror=return-type -Werror=switch

CFLAGS = -c -g -I$(GENERATED_INCLUDES_DIR) -I$(INCLUDE_DIR) -I$(LOG_C_INCLUDES) $(LOG_FLAGS) -Wall $(ERR_FLAGS) -MMD
LDFLAGS = -g -O3

$(warning $(OBJECTS))

all: bc

bc: $(EXE_FILE)

$(EXE_FILE): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS)

$(GENERATED_SOURCES): $(RE2_SOURCES)
	mkdir -p $(GENERATED_SOURCES_DIR)
	mkdir -p $(GENERATED_INCLUDES_DIR)
	$(RE2C) $< -o $@ $(RE2C_FLAGS)

build/objs/c/%.o: src/c/%.c
	mkdir -p $(OBJECT_FILE_DIR)
	mkdir -p $(OBJ_SUBDIRS)
	$(CC) -o $@ $< $(CFLAGS)

build/objs/c/%.re.o: $(GENERATED_SOURCES_DIR)/%.re.c
	$(CC) -o $@ $< $(CFLAGS)

$(LOG_C_OBJ): $(LOG_C_SOURCE_DIR)/log.c
	mkdir -p $(LOG_C_OBJS_DIR)
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf $(BUILDDIR)

-include $(DEPS)