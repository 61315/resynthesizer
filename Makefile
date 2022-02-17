.POSIX:
CC        = clang -std=c99
CPPFLAGS  = -MMD -MP -DSYNTH_LIB_ALONE
CFLAGS    = -Wall -Wextra -pedantic -O3
LDFLAGS   = 
LDLIBS    = 
# PREFIX = /usr/local

LIB_DIR := lib
BUILD_DIR := build
SRC_DIR := resynthesizer

# Collect resynthesizer sources and headers, then create object files out of the sources.
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

STATIC_LIB := $(LIB_DIR)/libresynthesizer.a

EXAMPLE_DIR := examples
EXAMPLES := $(EXAMPLE_DIR)/dummy $(EXAMPLE_DIR)/ppm

# -g -Wall -Wextra -Werror -std=c99 -pedantic-errors
# TODO: Try both -Werror and -pedantic-errors after all the chores are done.

all: $(STATIC_LIB) $(EXAMPLES)
	@echo "\033[1;92mDone!\033[0m"

# Build resynthesizer as static library.
$(STATIC_LIB): $(OBJS)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	ar rvs $@ $^

$(BUILD_DIR)/%.o: %.c
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build the example executables.
$(EXAMPLE_DIR)/dummy: examples/dummy.c $(STATIC_LIB)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ examples/dummy.c $(LDFLAGS) $(INC_FLAGS) $(STATIC_LIB)

$(EXAMPLE_DIR)/ppm: examples/ppm.c $(STATIC_LIB)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ examples/ppm.c $(LDFLAGS) $(INC_FLAGS) $(STATIC_LIB)

# Run the executable(ppm) against the sample images with varying parameters.
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

# Include resynthesizer headers from the `-MMD` and `-MP` flags.
-include $(DEPS)