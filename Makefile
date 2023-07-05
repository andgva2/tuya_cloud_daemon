SRC_DIR:=./src
LIB_DIR:=./lib
SDK_DIR:=./sdk

./PHONY: all clean

all:
	cd $(SDK_DIR) && $(MAKE) $@
	cd $(LIB_DIR) && $(MAKE) $@
	cd $(SRC_DIR) && $(MAKE) $@

clean:
	$(MAKE) -C $(LIB_DIR) $@
	$(MAKE) -C $(SRC_DIR) $@
	$(MAKE) -C $(SDK_DIR) $@
