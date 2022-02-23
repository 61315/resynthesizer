.POSIX:
CC        = clang -std=c99
CPPFLAGS  = -MMD -MP -DSYNTH_LIB_ALONE
CFLAGS    = -Wall -Wextra -pedantic -O3
LDFLAGS   = -lm
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

ASSET_DIR := assets
EXAMPLE_DIR := examples
EXAMPLES := $(EXAMPLE_DIR)/hello $(EXAMPLE_DIR)/ppm $(EXAMPLE_DIR)/painter

# -g -Wall -Wextra -Werror -std=c99 -pedantic-errors
# TODO: Try both -Werror and -pedantic-errors after all the chores are done.

all: $(STATIC_LIB) test
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
$(EXAMPLE_DIR)/hello: $(EXAMPLE_DIR)/hello.c $(STATIC_LIB)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(EXAMPLE_DIR)/hello.c $(LDFLAGS) $(INC_FLAGS) $(STATIC_LIB)

$(EXAMPLE_DIR)/ppm: $(EXAMPLE_DIR)/ppm.c $(STATIC_LIB)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(EXAMPLE_DIR)/ppm.c $(LDFLAGS) $(INC_FLAGS) $(STATIC_LIB)

$(EXAMPLE_DIR)/painter: $(EXAMPLE_DIR)/painter.c $(STATIC_LIB)
	@echo "\033[1;92mBuilding $@\033[0m"
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(EXAMPLE_DIR)/painter.c $(LDFLAGS) $(INC_FLAGS) $(STATIC_LIB) $(shell pkg-config --cflags --libs sdl2)

# Run the executable(ppm) against the sample images with varying parameters.
fuzz: $(EXAMPLE_DIR)/ppm
	@echo "\033[1;92mFuzzing...\033[0m"
	mkdir -p $(EXAMPLE_DIR)/output
	@for number in 0 1 2 3 4 ; do \
		for context in 0 1 2 3 4 5 6 7 8 ; do \
			for neighbors in 9 64 ; do \
				for probes in 64 256 ; do \
					$(EXAMPLE_DIR)/ppm \
					$(ASSET_DIR)/source00$${number}.ppm \
					$(ASSET_DIR)/mask00$${number}.ppm \
					$(EXAMPLE_DIR)/output/result00$${number}"_"$${context}"_"$${neighbors}"_"$${probes}.ppm \
					$${context} $${neighbors} $${probes} ; \
				done \
			done \
		done \
	done

test: $(EXAMPLE_DIR)/hello
	@echo "\033[1;92mTesting...\033[0m"
	@if $(EXAMPLE_DIR)/hello; then \
		echo "\033[1;92mTest Passed!\033[0m"; \
	else \
		echo "\033[1;91mTest Failed!\033[0m"; \
		exit 1; \
	fi

.PHONY: clean test all

clean:
	$(RM) -r $(BUILD_DIR) $(LIB_DIR) $(EXAMPLES)

# Include resynthesizer headers from the `-MMD` and `-MP` flags.
-include $(DEPS)
