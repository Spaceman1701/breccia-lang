
CC = clang-12
RE2C = re2c

RE2C_FLAGS = -W -Werror -i --utf8 --case-ranges

BUILDDIR = build
GENDIR = "$(BUILDDIR)/gen"

SRC_DIR = src

RE2_SOURCES = $(wildcard $(SRC_DIR)/re2c/*.re)
GENERATED_SOURCES = $(patsubst $(SRC_DIR)/re2c/%.re,$(GENDIR)/%.re.c,$(RE2_SOURCES))


$(info "$(GENDIR)")

all: $(GENERATED_SOURCES)

$(GENERATED_SOURCES): $(RE2_SOURCES) dirs
	$(RE2C) $< -o $@ $(RE2C_FLAGS)

dirs:
	mkdir -p $(BUILDDIR)
	mkdir -p $(GENDIR)

clean:
	rm -rf $(BUILDDIR)