INCLUDE_SDK_DIR:=../include/sdk
SDK_BUILD_LIBS:=./build/lib
LIBRARIES_DIR:=../lib
BUILD_DIR:=./build

.PHONY: all clean build

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
	$(RM) -r $(BUILD_DIR)
