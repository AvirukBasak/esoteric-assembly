CC          := gcc
DBG         := gdb -q
CFLAGS      := -Ofast
CDBGFLAGS   := -g -ggdb

LIB         := -lm

SRCEXT      := c
OBJEXT      := o

SRC_DIR     := src
BIN_DIR     := bin

TARGET      := $(BIN_DIR)/asm
DBG_TARGET  := $(BIN_DIR)/asm-dbg

SOURCES     := $(shell find $(SRC_DIR)/ | grep '\.$(SRCEXT)')
OBJDIRS     := $(patsubst $(SRC_DIR)%, $(BIN_DIR)%, $(shell dirname $(SOURCES)))

## release build

OBJECTS     := $(patsubst $(SRC_DIR)/%.$(SRCEXT), $(BIN_DIR)/%.$(OBJEXT), $(SOURCES))

all: $(TARGET)

$(TARGET): mkdirp $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIB)

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $(patsubst $(BIN_DIR)/%.$(OBJEXT), $(SRC_DIR)/%.$(SRCEXT), $@) -o $@

## debug build

DBG_OBJECTS  := $(patsubst $(SRC_DIR)/%.$(SRCEXT), $(BIN_DIR)/%-dbg.$(OBJEXT), $(SOURCES))

dbg: $(DBG_TARGET)

$(DBG_TARGET): mkdirp $(DBG_OBJECTS)
	$(CC) $(CDBGFLAGS) $(DBG_OBJECTS) -o $(DBG_TARGET) $(LIB)

$(DBG_OBJECTS): $(SOURCES)
	$(CC) $(CDBGFLAGS) -c $(patsubst $(BIN_DIR)/%-dbg.$(OBJEXT), $(SRC_DIR)/%.$(SRCEXT), $@) -o $@

## execution

run: $(TARGET)
	@./$(TARGET)
	
rundbg: $(DBG_TARGET)
	@$(DBG) -q $(DBG_TARGET)

## mkdirp

mkdirp:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJDIRS)

## Clean

clean:
	@rm -rf $(OBJECTS)
	@rm -rf $(DBG_OBJECTS)

cleaner:
	@rm -rf $(OBJECTS)
	@rm -rf $(DBG_OBJECTS)
	@rm -rf $(TARGET)
	@rm -rf $(DBG_TARGET)
