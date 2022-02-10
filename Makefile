CC = clang

TARGET_LIB := libresynthesizer.a

BUILD_DIR := build
SRC_DIRS := resynthesizer

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP
CFLAGS := -DSYNTH_LIB_ALONE -g2 -Wall -Wextra -std=c99 -pedantic -Ofast

# -g -Wall -Wextra -Werror -std=c99 -pedantic-errors
# TODO: Try both -Werror and -pedantic-errors after all the chores are done.

$(BUILD_DIR)/$(TARGET_LIB): $(OBJS)
	ar rvs $@ $^

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)