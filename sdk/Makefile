INCLUDE_SDK_DIR:=../include/sdk
SDK_BUILD_LIBS:=./build/lib
LIBRARIES_DIR:=../lib
LIBRARIES:=$(notdir $(wildcard $(LIBRARIES_DIR)/*.so))
BUILD_DIR:=./build
SYSTEM_LIBS_DIR:=/usr/lib

.PHONY: all clean build iqnstall uninstall

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && pwd && cmake ..
	cd $(BUILD_DIR) && make
	mkdir -p $(INCLUDE_SDK_DIR)
	cp $(wildcard ./include/*) $(wildcard ./interface/*) $(wildcard ./utils/*.h) $(INCLUDE_SDK_DIR)/
	mkdir -p $(LIBRARIES_DIR)
	cp -r $(SDK_BUILD_LIBS)/. $(LIBRARIES_DIR)
	
clean:
	$(RM) -r $(LIBRARIES_DIR)
	$(RM) -r $(BUILD_DIR)

install: build
	sudo cp -r $(LIBRARIES_DIR)/. $(SYSTEM_LIBS_DIR)/

uninstall: build
uninstall: build
	cd $(SYSTEM_LIBS_DIR) && sudo $(RM) $(LIBRARIES)
	$(MAKE) clean -C ./