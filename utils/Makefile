INCLUDE_DIR:=../include
BUILD_FLAGS:=-I$(INCLUDE_DIR) -c
SRC:=$(wildcard *.c)

OBJ:=$(SRC:.c=.o)
OBJ_DIR:=../obj
OBJ_TARGETS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

.PHONY: all clean

all: $(OBJ_TARGETS)

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(BUILD_FLAGS) -o $@ $^

clean:	
	$(RM) $(OBJ_TARGETS)
	rmdir $(OBJ_DIR) 2>/dev/null || true