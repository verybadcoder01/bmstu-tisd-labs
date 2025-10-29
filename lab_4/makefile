CC := gcc
CFLAGS := -Wall -Werror -Wpedantic -Wextra -Wfloat-equal -Wfloat-conversion -Iinc -std=gnu99 -g -O1
SAN_COMP := clang
SANITIZER_FLAGS_ADDRESS := -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer -g
SANITIZER_FLAGS_MEMORY := -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -g
SANITIZER_FLAGS := $(SANITIZER_FLAGS_ADDRESS)

SRC_DIR := src
UNIT_DIR := unit_tests
OUT_DIR := out
FUNC_TESTS_SCRIPT := func_tests/scripts/func_tests.sh

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
UNIT_FILES := $(wildcard $(UNIT_DIR)/*.c)

OBJS := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SRC_FILES))
UNIT_OBJS := $(patsubst $(UNIT_DIR)/%.c,$(OUT_DIR)/%.o,$(UNIT_FILES))

UNIT_EXE := unit_tests.exe
FUNC_EXE := app.exe
SAN_FUNC_EXE := san_app.exe

ifneq (,$(findstring i, $(MAKEFLAGS)))
	CFLAGS += -DDEBUG_PRINT
endif

.PHONY: all unit func clean test_san

all: $(FUNC_EXE)

$(OUT_DIR):
	mkdir -p $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.o: $(UNIT_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(UNIT_EXE): $(filter-out $(OUT_DIR)/main.o,$(OBJS)) $(UNIT_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(FUNC_EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(SAN_FUNC_EXE): $(OBJS)
	$(SAN_COMP) $(CFLAGS) $(SANITIZER_FLAGS) $^ -o $@

unit: $(UNIT_EXE)
	./$(UNIT_EXE)

func: $(FUNC_EXE)
	./$(FUNC_TESTS_SCRIPT)

test_san: $(SAN_FUNC_EXE)
	-./$(FUNC_TESTS_SCRIPT) -nv san_app.exe
	$(eval SANITIZER_FLAGS := $(SANITIZER_FLAGS_MEMORY))
	make clean
	make san_app.exe
	-./$(FUNC_TESTS_SCRIPT) -nv san_app.exe

clean:
	rm -f $(UNIT_EXE) $(FUNC_EXE) $(SAN_FUNC_EXE)
	rm -rf $(OUT_DIR)/*
	rm -f app_output_* neg_out_* *.gcda *.gcno *.gcov
