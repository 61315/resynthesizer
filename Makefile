.POSIX:
CC = cc
CPPFLAGS := -MMD -MP -DSYNTH_LIB_ALONE
CFLAGS := -Wall -Wextra -std=c99 -pedantic -Ofast
LDFLAGS = -s
# LDLIBS = 
# PREFIX = /usr/local

LIB_DIR := lib
BUILD_DIR := build
SRC_DIR := resynthesizer

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

TARGET_LIB := $(LIB_DIR)/libresynthesizer.a

EXAMPLE_DIR := examples
EXAMPLE_SRCS := $(shell find $(EXAMPLE_DIR) -name '*.c')
EXAMPLES := $(basename $(EXAMPLE_SRCS))

# -g -Wall -Wextra -Werror -std=c99 -pedantic-errors
# TODO: Try both -Werror and -pedantic-errors after all the chores are done.

all: $(EXAMPLES)
	@echo "\033[1;92mDone!\033[0m"

$(EXAMPLES): $(TARGET_LIB) $(EXAMPLE_SRCS)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $@.c $(LDFLAGS) $(INC_FLAGS) $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	ar rvs $@ $^

$(BUILD_DIR)/%.c.o: %.c
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

test: $(EXAMPLES)
	echo "\033[1;92mTesting...\033[0m"
	mkdir -p out
	@for number in 0 1 2 3 4 ; do \
		for context in 0 1 2 3 4 5 6 7 8 ; do \
			for neighbors in 9 64 ; do \
				for probes in 64 256 ; do \
					./examples/ppm \
					assets/source00$${number}.ppm \
					assets/mask00$${number}.ppm \
					out/result00$${number}"_"$${context}"_"$${neighbors}"_"$${probes}.ppm \
					$${context} $${neighbors} $${probes} ; \
				done \
			done \
		done \
	done

.PHONY: clean test all

clean:
	$(RM) -r $(BUILD_DIR) $(LIB_DIR) $(EXAMPLES)

-include $(DEPS)