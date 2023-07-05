LIB:=libbecome_daemon.so

SRC:=$(wildcard *.c)

OBJ:=$(SRC:.c=.o)
OBJ_DIR:=../obj

INCLUDE_DIR:=../include/become_daemon
CFLAGS:=-fPIC -shared -I$(INCLUDE_DIR)

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	mkdir -p $(OBJ_DIR)
	mv $(OBJ) $(OBJ_DIR)

clean:
	$(RM) $(LIB) $(OBJ_DIR)/$(OBJ) $(wildcard *.so)
	[ "$(ls -A $(OBJ_DIR))" ] && sleep 1s || $(RM) -r $(OBJ_DIR)
