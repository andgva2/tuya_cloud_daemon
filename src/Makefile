INCLUDE_DIR:=../include
BUILD_FLAGS:=-I$(INCLUDE_DIR)
OBJECT_BUILD_FLAGS:=-c -I$(INCLUDE_DIR)

BIN:=tuya_daemon
BIN_DIR:=../bin/
BIN_TARGET:=$(BIN_DIR)/$(BIN)
SRC:=$(wildcard *.c)

OBJ:=$(SRC:.c=.o)
OBJ_DIR:=../obj
OBJ_TARGET:=$(OBJ_DIR)/$(OBJ)

LIB_DIR:=../lib
LIB:=$(wildcard $(LIB_DIR)/*.so)
LIB_FLAGS:=$(addprefix -l,$(subst lib,,$(notdir $(basename $(LIB)))))
LDFLAGS:=-L$(LIB_DIR) $(LIB_FLAGS) -Wl,-rpath=$(LIB_DIR)

.PHONY: all clean

all: $(BIN_TARGET) $(OBJ_TARGET)

$(BIN_TARGET): $(OBJ_TARGET)
	mkdir -p $(BIN_DIR)
	$(CC) $(BUILD_FLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_TARGET): $(SRC)
	mkdir -p $(OBJ_DIR)
	$(CC) $(OBJECT_BUILD_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) -r $(OBJ_DIR)/$(OBJ) $(BIN_DIR)
	[ "$(ls -A $(OBJ_DIR))" ] && sleep 1s || $(RM) -r $(OBJ_DIR)

