# # ==== Compiler & toolchain ====
# CXX := mpicxx

# # check if mpicxx is available
# MPI_CXX_PATH := $(shell command -v mpicxx 2>/dev/null)
# ifndef MPI_CXX_PATH
#   $(error mpicxx not found. Please install MPI to run this command.)
# endif

# # ==== Flags (do NOT export env here) ====
# UNAME_S := $(shell uname -s)

# # Common flags
# CXXFLAGS := -std=c++17 -Iinclude
# LDFLAGS  :=
# LDLIBS   :=

# # Build type
# DEBUG ?= 0
# ifeq ($(DEBUG),1)
#   CXXFLAGS += -g
# else
#   CXXFLAGS += -O3
# endif

# ifeq ($(UNAME_S),Darwin)
#   CXXFLAGS += -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include
#   LDFLAGS  += -L/opt/homebrew/opt/libomp/lib -Wl,-rpath,/opt/homebrew/opt/libomp/lib
#   LDLIBS   += -lomp
# else
#   CXXFLAGS += -fopenmp
# endif


# # ==== Directories ====
# SRC_DIR  := src
# TEST_DIR := test
# BUILD_DIR := build
# BUILD_GTEST_DIR := $(BUILD_DIR)/gtest
# BUILD_GMOCK_DIR := $(BUILD_DIR)/gmock

# SRC_FILES := $(shell find $(SRC_DIR) -name '*.cc')
# OBJ_FILES := $(patsubst $(SRC_DIR)/%.cc,$(BUILD_DIR)/%.o,$(SRC_FILES))

# MAIN_SRC_FILE := spellcheck.cc
# TEST_SRC_FILE := $(TEST_DIR)/test_main.cc

# # ==== GTest / GMock ====
# GTEST_FLAGS := -Igtest/include -Igtest
# GMOCK_FLAGS := -Igmock/include -Igmock
# GTEST_SRCS  := gtest/src/gtest-all.cc
# GMOCK_SRCS  := gmock/src/gmock-all.cc
# GTEST_OBJS  := $(BUILD_GTEST_DIR)/gtest-all.o
# GMOCK_OBJS  := $(BUILD_GMOCK_DIR)/gmock-all.o

# # ==== Default ====
# all: spellcheck test format

# # ==== Compile rules ====
# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
# 	@mkdir -p $(dir $@)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(GTEST_OBJS): $(GTEST_SRCS)
# 	@mkdir -p $(BUILD_GTEST_DIR)
# 	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) -c $< -o $@

# $(GMOCK_OBJS): $(GMOCK_SRCS)
# 	@mkdir -p $(BUILD_GMOCK_DIR)
# 	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) $(GMOCK_FLAGS) -c $< -o $@

# # ==== Link targets ====
# spellcheck: $(OBJ_FILES) $(MAIN_SRC_FILE)
# 	@mkdir -p $(BUILD_DIR)
# 	$(CXX) $(CXXFLAGS) $(OBJ_FILES) $(MAIN_SRC_FILE) -o $(BUILD_DIR)/$@ $(LDFLAGS) $(LDLIBS)

# test: $(TEST_SRC_FILE) $(GTEST_OBJS) $(GMOCK_OBJS) $(OBJ_FILES)
# 	@mkdir -p $(BUILD_DIR)
# 	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) $(GMOCK_FLAGS) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS) $(LDLIBS)
# 	$(BUILD_DIR)/test

# # ==== Format ====
# CLANG_FORMAT_PATH := $(shell command -v clang-format 2>/dev/null)
# format:
# ifdef CLANG_FORMAT_PATH
# 	$(CLANG_FORMAT_PATH) -i $(SRC_FILES) $(MAIN_SRC_FILE) $(TEST_SRC_FILE)
# endif

# # ==== Archive ====
# VER ?=
# archive:
# 	@git archive --format=tar.gz --prefix skeleton-p2/ -o comp90025-skeleton-p2-v$(VER).tar.gz HEAD

# # ==== Clean ====
# clean:
# 	rm -rf $(BUILD_DIR)/*


CC = mpiCC

# check if mpiCC is available
MPI_CC_PATH := $(shell command -v mpiCC 2>/dev/null)
ifndef MPI_CC_PATH
	$(error mpiCC not found. Please install MPI to run this command.)
endif

# Check if the machine is macOS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    IS_MACOS := 1
    # Set environment variables for Homebrew-installed LLVM
    export PATH := /opt/homebrew/opt/llvm/bin:$(PATH)
    export LDFLAGS := -L/opt/homebrew/opt/llvm/lib -L/opt/homebrew/lib -lomp
    export CCFLAGS := -I/opt/homebrew/opt/llvm/include -I/opt/homebrew/include -Xpreprocessor -fopenmp
else
    IS_MACOS := 0
    LDFLAGS = -lmpi -fopenmp
endif


# Set DEBUG to 1 for debug build, 0 for release build
DEBUG ?= 0
CCFLAGS += -std=c++17 -Iinclude 
GTEST_FLAGS = -Igtest/include -Igtest
GMOCK_FLAGS = -Igmock/include -Igmock

ifeq ($(DEBUG), 1)
    CCFLAGS += -g
else
    CCFLAGS += -O3
endif

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
BUILD_GTEST_DIR = $(BUILD_DIR)/gtest
BUILD_GMOCK_DIR = $(BUILD_DIR)/gmock

SRC_FILES := $(shell find $(SRC_DIR) -name '*.cc')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cc,$(BUILD_DIR)/%.o,$(SRC_FILES))

MAIN_SRC_FILE := spellcheck.cc
TEST_SRC_FILE := $(TEST_DIR)/test_main.cc

# GTest
GTEST_SRCS = gtest/src/gtest-all.cc
GTEST_MAIN_SRCS = gtest/src/gtest_main.cc
GTEST_OBJS = $(BUILD_GTEST_DIR)/gtest-all.o
GMOCK_SRCS = gmock/src/gmock-all.cc
GMOCK_OBJS = $(BUILD_GMOCK_DIR)/gmock-all.o

# Default target
all: spellcheck test format

# GTest object files
$(GTEST_OBJS): $(GTEST_SRCS)
	@mkdir -p $(BUILD_GTEST_DIR)
	$(CC) $(CCFLAGS) $(GTEST_FLAGS) -c $< -o $(GTEST_OBJS)

# GMock object files
$(GMOCK_OBJS): $(GMOCK_SRCS) 
	@mkdir -p $(BUILD_GMOCK_DIR)
	$(CC) $(CCFLAGS) $(GTEST_FLAGS) $(GMOCK_FLAGS) -c $< -o $(GMOCK_OBJS)

# Rule to compile each .cc file to its corresponding .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CC)  $(CCFLAGS) -c $< -o $@

test: $(TEST_SRC_FILE) $(GTEST_OBJS) $(GMOCK_OBJS) $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CCFLAGS) $(GTEST_FLAGS) $(GMOCK_FLAGS) $^ $(LDFLAGS) -o $(BUILD_DIR)/$@
	build/test

spellcheck: $(OBJ_FILES) $(MAIN_SRC_FILE)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) -lstdc++ $(OBJ_FILES) $(MAIN_SRC_FILE) -o $(BUILD_DIR)/$@

CLANG_FORMAT_PATH := $(shell command -v clang-format 2>/dev/null)
format:
ifdef CLANG_FORMAT_PATH
	$(CLANG_FORMAT_PATH) -i $(SRC_FILES) $(MAIN_SRC_FILE) $(TEST_SRC_FILE)
else
# 	@echo "clang-format not found. Please install clang-format to run this command."
endif

VER ?=
archive:
	@git archive --format=tar.gz --prefix skeleton-p2/ -o comp90025-skeleton-p2-v$(VER).tar.gz HEAD

clean:
	rm -rf $(BUILD_DIR)/*


