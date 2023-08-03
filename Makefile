CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra
CXXFLAGS_DEBUG := -O0 -g -std=c++11 -Wall -ggdb3

VALGRIND := valgrind

BIN_PATH := bin
OBJ_PATH := obj
BIN_DEBUG_PATH := bin/debug
SRC_PATH := src

COMPILER_PATH := src/compiler
DEBUG_PATH := src/debug
GC_PATH := src/garbage_collector
MEMORY_PATH := src/memory
NATIVEFN_PATH := src/native
SRC_PARSER_PATH := src/parser
TYPE_PATH := src/types
VM_PATH := src/vm

TARGET := $(BIN_PATH)
TARGET_DEBUG := $(BIN_DEBUG_PATH)

RM := rm

ifeq ($(OS),Windows_NT)
	TARGET := $(BIN_PATH)\zura.exe
	RM := del /q 
endif

SOURCE_FILES := $(wildcard $(SRC_PATH)/*.cpp) $(wildcard $(GC_PATH)/*.cpp) $(wildcard $(SRC_PARSER_PATH)/*.cpp) $(wildcard $(DEBUG_PATH)/*.cpp) $(wildcard $(MEMORY_PATH)/*.cpp) $(wildcard $(VM_PATH)/*.cpp) $(wildcard $(COMPILER_PATH)/*.cpp) $(wildcard $(NATIVEFN_PATH)/*.cpp) $(wildcard $(TYPE_PATH)/*.cpp)

workflow:
# --> Linux 
	@$(CXX) -o zura $(SOURCE_FILES) $(CXXFLAGS)
# --> Windows GCC
	@$(CXX) -o zura.exe $(SOURCE_FILES) $(CXXFLAGS)
