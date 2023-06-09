CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra

BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src

TARGET := $(BIN_PATH)/main

RM := rm

ifeq ($(OS),Windows_NT)
	TARGET := $(BIN_PATH)\azura.exe
	RM := del /f
endif

PWD := $(shell echo %cd%)

SOURCE_FILES := $(wildcard $(SRC_PATH)/*.cpp)

all:
	$(CXX) -o $(TARGET) $(SOURCE_FILES) $(CXXFLAGS)

clean:
	$(RM) $(TARGET)
