ROOT_DIR := ../..
BUILD_DIR := $(ROOT_DIR)/build

.PHONY: all clean

all:
	cmake -S $(ROOT_DIR) -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR) --target dinning --parallel

clean: 
	rm -rf $(BUILD_DIR)
