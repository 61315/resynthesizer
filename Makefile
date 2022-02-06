CC = clang
TARGET_LIB := libresynthesizer.a

BUILD_DIR := build
SRC_DIRS := resynthesizer

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := $(INC_FLAGS) -MMD -MP -DSYNTH_LIB_ALONE

$(BUILD_DIR)/$(TARGET_LIB): $(OBJS)
	ar rvs $@ $^

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)