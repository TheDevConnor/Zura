# =============================================================================
#
#                             Zura Makefile
#
# =============================================================================
# This Makefile supports the Zura project. It can build on Linux using GCC,
# Windows using GCC derived from the MinGW project, and Windows using MSVC.
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


BIN_DIR   = bin
INC_DIR   = inc
OBJ_DIR   = obj

OBJ_FILES  = $(patsubst %.cpp,%.obj,$(SRC_FILES))


# -----------------------------------------------------------------------------
# Default build 
# -----------------------------------------------------------------------------

CXX          = g++ --std=c++20
CXX_EMIT_OBJ = -c -o
CXX_EMIT_EXE = -o
CXX_INC      = -I
CXX_LIB      = -L
CXX_MACRO    = -D

LIBS         = -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl

# TODO: Test these
CLEAN_OBJ = find test -type f -name "*.obj" -delete
CLEAN_EXE = rm ./bin/zura ./bin/debug ./bin/imgui_demo

ECHO = echo


# -----------------------------------------------------------------------------
# Windows-specific variable-reassignment
# -----------------------------------------------------------------------------
ifeq ($(OS), Windows_NT)

CXX     += $(CXX_MACRO)_CRT_SECURE_NO_WARNINGS

LIB_DIR = .\lib\win\mingw-w64
LIBS    = -lmingw32 -lopengl32 -lglfw3 -lshell32 -luser32 -lgdi32

# Should work with PowerShell Version 5 and above.
CLEAN_OBJ = powershell.exe -noprofile -Command " & {Get-ChildItem -recurse *.obj | Remove-Item}"
CLEAN_EXE = powershell.exe -noprofile -Command " & {Get-ChildItem -recurse *.exe | Remove-Item}"

endif

# -----------------------------------------------------------------------------
# Windows Build with MSVC
# -----------------------------------------------------------------------------
ifeq ($(MSVC), 1)

CXX          = cl.exe /std:c++20 /EHsc /nologo /MD
CXX_INC      = /I
CXX_EMIT_OBJ = /c /Fo:
CXX_EMIT_EXE = /Fe:
CXX_LIB      = /link /LIBPATH:

CXX_MACRO    = /D

CXX           += $(CXX_MACRO)_CRT_SECURE_NO_WARNINGS

LIB_DIR = .\lib\win\vc2022

LIBS = opengl32.lib glfw3.lib shell32.lib user32.lib gdi32.lib

endif

# -----------------------------------------------------------------------------
# Additional Compilation Flags
# -----------------------------------------------------------------------------
ifeq ($(ZURA_GUI),1)
CXX += $(CXX_MACRO)ZURA_GUI
SRC_FILES += $(IMGUI_SRC_FILES)
endif

ifeq ($(IMGUI_DEMO_WINDOW),1)
CXX += $(CXX_MACRO)IMGUI_DEMO_WINDOW
endif

# -----------------------------------------------------------------------------
# Build Recipes
# -----------------------------------------------------------------------------

# Non-File Targets
.PHONY: zura debug zura_gui zura_gui_debug clean imgui_demo

imgui_demo: $(IMGUI_DEMO_SRC:.cxx=.obj) $(IMGUI_OBJ_FILES)
	$(CXX) $(CXX_INC)$(INC_DIR) $^ $(LIBS) $(CXX_EMIT_EXE)$(BIN_DIR)/$@ $(CXX_LIB)$(LIB_DIR) 

zura: $(OBJ_FILES)
	$(CXX) $(CXX_INC)$(INC_DIR) $^ $(LIBS) $(CXX_EMIT_EXE)$(BIN_DIR)/$@ $(CXX_LIB)$(LIB_DIR) 

%.obj : %.cpp
	$(CXX) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_OBJ)$@

%.obj : %.cxx
	$(CXX) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_OBJ)$@

clean: 
	@$(CLEAN_OBJ)
	@$(CLEAN_EXE)
	
echo_OS: 
	@echo $(OS)

echo_SRC: 
	@echo $(SRC_FILES)


