CXX := g++
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra -D_CRT_SECURE_NO_WARNINGS -Wno-missing-field-initializers
CXXFLAGS_DEBUG := -O0 -g -std=c++17 -Wall -D_CRT_SECURE_NO_WARNINGS

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
SRC_PARSER_LEXER_PATH := src/parser/lexer
TYPE_PATH := src/types
VM_PATH := src/vm

TARGET := $(BIN_PATH)
TARGET_DEBUG := $(BIN_DEBUG_PATH)

RM := rm

ifeq ($(OS),Windows_NT)
	TARGET := $(BIN_PATH)\zura.exe
	CXXFLAGS +=  -lws2_32 -lmingw32 -liphlpapi
	RM := del /q 
endif

SOURCE_FILES := $(wildcard $(SRC_PATH)/*.cpp) $(wildcard $(GC_PATH)/*.cpp) $(wildcard $(SRC_PARSER_PATH)/*.cpp) $(wildcard $(DEBUG_PATH)/*.cpp) $(wildcard $(MEMORY_PATH)/*.cpp) $(wildcard $(VM_PATH)/*.cpp) $(wildcard $(COMPILER_PATH)/*.cpp) $(wildcard $(NATIVEFN_PATH)/*.cpp) $(wildcard $(TYPE_PATH)/*.cpp) $(wildcard $(SRC_PARSER_LEXER_PATH)/*.cpp)

windows: 
	@$(CXX) -o $(TARGET) $(SOURCE_FILES) $(CXXFLAGS)
linux:
	@$(CXX) -o $(TARGET)/zura $(SOURCE_FILES) $(CXXFLAGS)
debug:
	@$(CXX) -o $(TARGET_DEBUG)/debug $(SOURCE_FILES) $(CXXFLAGS_DEBUG)

workflow:
# --> Linux 
	@$(CXX) -o zura $(SOURCE_FILES) $(CXXFLAGS)
# --> Windows GCC
	@$(CXX) -o zura.exe $(SOURCE_FILES) $(CXXFLAGS)
