CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra
CXXFLAGS_DEBUG := -O0 -g -std=c++11 -Wall -ggdb3

VALGRIND := valgrind

BIN_PATH := bin
BIN_DEBUG_PATH := bin/debug
SRC_PATH := src
GC_PATH := src/garbage_collector
SRC_PARSER_PATH := src/parser
DEBUG_PATH := src/debug
MEMORY_PATH := src/memory

TARGET := $(BIN_PATH)
TARGET_DEBUG := $(BIN_DEBUG_PATH)

RM := rm

ifeq ($(OS),Windows_NT)
	TARGET := $(BIN_PATH)\zura.exe
	RM := del /q 
endif

SOURCE_FILES := $(wildcard $(SRC_PATH)/*.cpp*) $(wildcard $(GC_PATH)/*.cpp) $(wildcard $(SRC_PARSER_PATH)/*.cpp*) $(wildcard $(DEBUG_PATH)/*.cpp*) $(wildcard $(MEMORY_PATH)/*.cpp*)

workflow:
# --> Linux 
	@$(CXX) -o zura $(SOURCE_FILES) $(CXXFLAGS)
# --> Windows
	@$(CXX) -o zura.exe $(SOURCE_FILES) $(CXXFLAGS)

debug:
	@echo Cleaning...
	@$(RM) -f $(wildcard $(BIN_DEBUG_PATH)/*)
	@echo Cleaned

	@python progress_bar.py $(CXX) -o $(TARGET_DEBUG)/zura $(SOURCE_FILES) $(CXXFLAGS_DEBUG)

linux:
	@echo Cleaning...
	@$(RM) -f $(wildcard $(BIN_PATH)/*)
	@echo Cleaned

	@python progress_bar.py $(CXX) -o $(TARGET)/zura $(SOURCE_FILES) $(CXXFLAGS)

windows:
	@echo Cleaning...
	@$(RM) $(BIN_PATH)
	@echo Cleaned

	@python progress_bar.py $(CXX) -o $(TARGET) $(SOURCE_FILES) $(CXXFLAGS)

valgrind:
	@echo Checking for memory leaks
	$(VALGRIND) -v --leak-check=full --show-leak-kinds=all bin/zura > --log-file=valgrind.log 2>&1
