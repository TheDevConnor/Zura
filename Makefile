CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra

BIN_PATH := bin
SRC_PATH := src
SRC_PARSER_PATH := src/parser
DEBUG_PATH := src/debug

TARGET := $(BIN_PATH)

RM := rm

ifeq ($(OS),Windows_NT)
	TARGET := $(BIN_PATH)\azura.exe
endif

PWD := $(shell echo %cd%)

SOURCE_FILES := $(wildcard $(SRC_PARSER_PATH)/*.cpp*) $(wildcard $(DEBUG_PATH)/*.cpp*)

MAIN_FILE := $(wildcard $(SRC_PATH)/main.cpp)

all:
	# This was a bitch to fix
	$(CXX) -o $(TARGET) $(SOURCE_FILES) $(MAIN_FILE) $(CXXFLAGS)
