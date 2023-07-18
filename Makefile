CXX := g++
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra
CXXFLAGS_DEBUG := -O0 -g -std=c++11 -Wall -ggdb3

VALGRIND := valgrind

BIN_PATH := bin
OBJ_PATH := obj
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

SOURCE_FILES := $(wildcard $(SRC_PATH)/*.cpp) $(wildcard $(GC_PATH)/*.cpp) $(wildcard $(SRC_PARSER_PATH)/*.cpp) $(wildcard $(DEBUG_PATH)/*.cpp) $(wildcard $(MEMORY_PATH)/*.cpp)


workflow:
# --> Linux 
	@$(CXX) -o zura $(SOURCE_FILES) $(CXXFLAGS)
# --> Windows GCC
	@$(CXX) -o zura.exe $(SOURCE_FILES) $(CXXFLAGS)

debug:
	@echo Cleaning...
	@$(RM) -f $(wildcard $(BIN_DEBUG_PATH)/*)
	@echo Cleaned

	@python3 progress_bar.py $(CXX) -o $(TARGET_DEBUG)/zura $(SOURCE_FILES) $(CXXFLAGS_DEBUG)

linux:
	@mkdir -p bin/
	@echo Cleaning...
	@$(RM) -f $(wildcard $(BIN_PATH)/*)
	@echo Cleaned

	@python3 progress_bar.py $(CXX) -o $(TARGET)/zura $(SOURCE_FILES) $(CXXFLAGS)

windows:
	@echo Cleaning...
	@$(RM) $(BIN_PATH)
	@echo Cleaned

	@python3 progress_bar.py $(CXX) -o $(TARGET) $(SOURCE_FILES) $(CXXFLAGS)

valgrind:
	@echo Checking for memory leaks
	$(VALGRIND) -v --leak-check=full --show-leak-kinds=all bin/zura > --log-file=valgrind.log 2>&1

# For x64-windows only
# Build then Link
OBJ_FILES    := $(wildcard $(OBJ_PATH)*.obj) $(wildcard $(OBJ_PATH)*.o)
PDB_FILES    := $(wildcard *.pdb)
EXE_FILES    := $(wildcard $(BIN_PATH)*.exe)
LIB_FILES    := $(wildcard *.lib)
CLEAN_FILES  := $(OBJ_FILES) $(PDB_FILES) $(EXE_FILES) $(LIB_FILES)

.PHONY: x64-windows-clean x64-windows-debug x64-windows-clean

x64-windows-debug:
	cl.exe /Od /EHsc /fsanitize=address /Zi /Fo:$(OBJ_PATH) /c $(SOURCE_FILES) 
	cl.exe /Od /EHsc /fsanitize=address /Zi $(OBJ_FILES) /Fe:$(BIN_PATH)zura-debug.exe $(SOURCE_FILES) 

x64-windows-release:
	cl.exe /O2 /EHsc /Fo:$(OBJ_PATH) /c $(SOURCE_FILES) 
	cl.exe /Od /EHsc  $(OBJ_FILES) /Fe:$(BIN_PATH)zura.exe $(SOURCE_FILES) 

x64-windows-clean: 
	@$(RM) $(CLEAN_FILES)
