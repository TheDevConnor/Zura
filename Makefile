CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra

BIN_PATH := bin
SRC_PATH := src
SRC_PARSER_PATH := src/parser
DEBUG_PATH := src/debug
MEMORY_PATH := src/memory

TARGET := $(BIN_PATH)

RM := rm

ifeq ($(OS),Windows_NT)
	TARGET := $(BIN_PATH)\azura.exe
	RM := del /q
endif

SOURCE_FILES := $(wildcard $(SRC_PARSER_PATH)/*.cpp*) $(wildcard $(DEBUG_PATH)/*.cpp*) $(wildcard $(MEMORY_PATH)/*.cpp*)

MAIN_FILE := $(wildcard $(SRC_PATH)/main.cpp)

all: build_progress

build_progress:
	@echo Cleaning...
	@$(RM) $(BIN_PATH)\*.exe
	@echo Clean complete!

	@python progress_bar.py $(CXX) -o $(TARGET) $(SOURCE_FILES) $(MAIN_FILE) $(CXXFLAGS)
