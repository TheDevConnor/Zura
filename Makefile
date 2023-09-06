# =============================================================================
#
#                             Zura Makefile
#
# =============================================================================
#
# This Makefile supports the Zura project. It can build on Linux + GCC,
# Windows + GCC, and Windows + MSVC.
#
# Building for Linux and Windows with GCC is straightforward.
#
# 	Example:
#               make clean
#               make zura
#
# 
# Building for Windows with MSVC requires an additional flag.
#
# 	Example:
#               make clean
#               make zura MSVC=1
#
#
# Building for Windows with MSVC with Dear ImGui using multiple jobs.
#
# 	Example:
#               make clean
#               make zura MSVC=1 ZURA_GUI=1 -j 12
#
# Building a debug build for Linux without Dear ImGui using jobs.
#
# 	Example:
#               make clean
#               make zura-cli-debug -j 12
#
#
# It is recommended you run `make clean` before changing your build type,
# ie. do not run `make zura` then `make debug` while modifying the source
# code. This may introduce errors during linking.
#
# =============================================================================


# -----------------------------------------------------------------------------
# Build variables
# Source files can be added here. 
# Append using the += operator as seen below.
# -----------------------------------------------------------------------------

SRC_FILES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)

IMGUI_SRC_FILES  = $(wildcard inc/imgui/*.cpp) $(wildcard inc/imgui/**/*.cpp)
IMGUI_OBJ_FILES  = $(patsubst %.cpp,%.obj,$(IMGUI_SRC_FILES))
IMGUI_DEMO_SRC = .\inc\imgui\opengl_demo_main.cxx


BIN_DIR   = zbin
INC_DIR   = inc
OBJ_DIR   = obj

OBJ_FILES  = $(patsubst %.cpp,%.obj,$(SRC_FILES))

# -----------------------------------------------------------------------------
# GCC Default build 
# -----------------------------------------------------------------------------

# Recommended GCC optimization is O2. O3 can be applied to specific parts of
# code after profiling.
CXX              = g++
CXX_FLAGS        = --std=c++20 -O2
CXX_DEBUG_FLAGS  = --std=c++20 -O0 -g -pedantic
CXX_EMIT_OBJ     = -c -o
CXX_EMIT_EXE     = -o
CXX_INC          = -I
# CXX_LIB          = -L
CXX_MACRO_PREFIX = -D

LIBS         = -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl

# TODO: Test these
CLEAN_OBJ = find . -type f -name "*.obj" -delete
CLEAN_EXE = rm ./bin/zura ./bin/debug ./bin/imgui_demo

ECHO = echo


# -----------------------------------------------------------------------------
# Windows-specific variable-reassignment
# -----------------------------------------------------------------------------
ifeq ($(OS), Windows_NT)

CXX     += $(CXX_MACRO_PREFIX)_CRT_SECURE_NO_WARNINGS

LIB_DIR = .\lib\win\mingw-w64
LIBS    = -lmingw32 -lopengl32 -lglfw3 -lshell32 -luser32 -lgdi32

# Should work with PowerShell Version 5 and above.
# Simply add additional file extensions here to add to clean recipe.
_CLEAN_FILES = pdb, obj, exe, ilk

CLEAN_FILES = $(addprefix *.,$(_CLEAN_FILES))
CLEAN_ALL = powershell.exe -noprofile -Command " & {Get-ChildItem -recurse $(CLEAN_FILES) | Remove-Item}"

endif

# -----------------------------------------------------------------------------
# Windows Build with MSVC
# -----------------------------------------------------------------------------
ifeq ($(MSVC), 1)

CXX          	 = cl.exe 
CXX_FLAGS        = /std:c++20 /EHsc /nologo /MD /O2
CXX_DEBUG_FLAGS  = /std:c++20 /EHsc /nologo /MD /W3 /Od /Z7 # /fsanitize=address
CXX_INC          = /I
CXX_EMIT_OBJ     = /c /Fo:
CXX_EMIT_EXE     = /Fe:
CXX_LIB          = /link /LIBPATH:

CXX_MACRO_PREFIX    = /D

CXX           += $(CXX_MACRO_PREFIX)_CRT_SECURE_NO_WARNINGS

LIB_DIR = .\lib\win\vc2022

LIBS = opengl32.lib glfw3.lib shell32.lib user32.lib gdi32.lib

endif

# -----------------------------------------------------------------------------
# Additional Compilation Flags
# -----------------------------------------------------------------------------

ifeq ($(ZURA_GUI), 1)
CXX        += $(CXX_MACRO_PREFIX)ZURA_GUI
endif

SRC_GUI     = $(wildcard src/gui/*.cpp)
OBJ_GUI     = $(SRC_GUI:.cpp=.obj)
OBJ_NO_GUI  = $(filter-out $(OBJ_GUI), $(OBJ_FILES))

# -----------------------------------------------------------------------------
# Build Recipes
# -----------------------------------------------------------------------------

# Non-File Targets
.PHONY: zura debug zura_gui zura_gui_debug clean imgui_demo

imgui_demo: $(IMGUI_DEMO_SRC:.cxx=.obj) $(IMGUI_OBJ_FILES)
	$(CXX) $(CXX_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(LIBS) $(CXX_EMIT_EXE)$(BIN_DIR)/$@ $(CXX_LIB)$(LIB_DIR) 

zura-gui: $(OBJ_FILES) $(IMGUI_OBJ_FILES)
	$(CXX) $(CXX_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(LIBS) $(CXX_EMIT_EXE)$(BIN_DIR)/$@ $(CXX_LIB)$(LIB_DIR) 

zura-cli: $(OBJ_NO_GUI)
	$(CXX) $(CXX_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_EXE)$(BIN_DIR)/$@ 

zura-gui-debug: $(OBJ_FILES:.obj=-d.obj) $(IMGUI_OBJ_FILES:.obj=-d.obj)
	$(CXX) $(CXX_DEBUG_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(LIBS) $(CXX_EMIT_EXE)$(BIN_DIR)/$@ $(CXX_LIB)$(LIB_DIR) 

zura-cli-debug: $(OBJ_NO_GUI:.obj=-d.obj)
	$(CXX) $(CXX_DEBUG_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_EXE)$(BIN_DIR)/$@

%-d.obj : %.cpp
	$(CXX) $(CXX_DEBUG_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_OBJ)$@

%.obj : %.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_OBJ)$@

%.obj : %.cxx
	$(CXX) $(CXX_FLAGS) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_OBJ)$@

clean: 
	$(CLEAN_OBJ)
	$(CLEAN_EXE)
	
echo_OS: 
	@echo $(OS)