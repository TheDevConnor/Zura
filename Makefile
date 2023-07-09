CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra

VALGRIND := valgrind

BIN_PATH := bin
SRC_PATH := src
GC_PATH := src/garbage_collector
SRC_PARSER_PATH := src/parser
DEBUG_PATH := src/debug
MEMORY_PATH := src/memory

TARGET := $(BIN_PATH)

RM := rm

ifeq ($(OS),Windows_NT)
	TARGET := $(BIN_PATH)\zura.exe
	RM := del /q 
endif

SOURCE_FILES := $(wildcard $(SRC_PATH)/*.cpp*) $(wildcard $(GC_PATH)/*.cpp) $(wildcard $(SRC_PARSER_PATH)/*.cpp*) $(wildcard $(DEBUG_PATH)/*.cpp*) $(wildcard $(MEMORY_PATH)/*.cpp*)

workflow:
	@$(CXX) -o zura $(SOURCE_FILES) $(CXXFLAGS)

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
	$(VALGRIND) ./zura
