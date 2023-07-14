INCLUDE_DIR:=../include
BUILD_FLAGS:=-I$(INCLUDE_DIR)
OBJECT_BUILD_FLAGS:=-c -I$(INCLUDE_DIR)

BIN:=tuya_daemon
BIN_DIR:=../bin
BIN_TARGET:=$(BIN_DIR)/$(BIN)
SYSTEM_BIN_DIR:=/usr/local/bin
UTILS_HEADERS:=$(wildcard $(INCLUDE_DIR)/utils/*.h)
SRC:=$(wildcard *.c)

OBJ:=$(SRC:.c=.o)
OBJ_DIR:=../obj
OBJ_TARGET:=$(OBJ_DIR)/$(OBJ)
OBJS:=$(wildcard $(OBJ_DIR)/*.o)

LIB_DIR:=../lib
LIB:=$(wildcard $(LIB_DIR)/*.so)
LIB_FLAGS:=$(addprefix -l,$(subst lib,,$(notdir $(basename $(LIB)))))
LDFLAGS:=-L$(LIB_DIR) $(LIB_FLAGS)

.PHONY: all clean install uninstall

all: $(BIN_TARGET)

$(BIN_TARGET): $(OBJ_TARGET) $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(BUILD_FLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_TARGET): $(SRC)
	mkdir -p $(OBJ_DIR)
	$(CC) $(OBJECT_BUILD_FLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) -r $(OBJ_TARGET) $(BIN_DIR)
	rmdir $(OBJ_DIR) 2>/dev/null || true

install: all
	sudo mkdir -p $(SYSTEM_BIN_DIR)
	sudo cp $(BIN_TARGET) $(SYSTEM_BIN_DIR)/

uninstall:
	sudo $(RM) $(SYSTEM_BIN_DIR)/$(BIN)