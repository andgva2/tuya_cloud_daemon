SDK_DIR:=./sdk
UTILS_DIR:=./utils
SRC_DIR:=./src

./PHONY: all clean install uninstall

all:
	$(MAKE) -C $(SDK_DIR)
	$(MAKE) -C $(UTILS_DIR)
	$(MAKE) -C $(SRC_DIR)

install:
	$(MAKE) $@ -C $(SDK_DIR)
	$(MAKE) $@ -C $(SRC_DIR)

uninstall:
	$(MAKE) $@ -C $(SDK_DIR)
	$(MAKE) $@ -C $(SRC_DIR)

clean:
	$(MAKE) $@ -C $(SRC_DIR)
	$(MAKE) $@ -C $(UTILS_DIR)
	$(MAKE) $@ -C $(SDK_DIR)
