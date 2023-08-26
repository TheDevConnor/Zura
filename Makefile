# =============================================================================
#
# 								Zura Makefile
#
# =============================================================================
# This Makefile supports the Zura project. It can build on Linux using GCC,
# Windows using GCC derived from the MinGW project, and Windows using MSVC.
#
# Building for Linux and Windows with GCC is straightforward.
#
# 	Example:
# 				make clean
# 				make zura
#
# 
# Building for Windows with MSVC requires an addtional flag.
#
# 	Example:
# 				make clean
# 				make zura MSVC=1
#
#
# It is recommended you run `make clean` before changing your build type,
# ie. do not run `make zura` then `make debug` while modifying the source
# code. This may introduce errors during linking.
#
# =============================================================================


# -----------------------------------------------------------------------------
# Default build 
# -----------------------------------------------------------------------------

CXX 		 =  g++ --std=c++20
CXX_EMIT_OBJ = -c -o
CXX_EMIT_EXE = -o
CXX_INC      = -I
CXX_LIB      = -L

LIBS 		 = -lglfw -X11

RM			 = rm


# -----------------------------------------------------------------------------
# Windows-specific variable-reassignment
# -----------------------------------------------------------------------------
ifeq ($(OS), Windows_NT)

LIB_DIR = .\lib\win\mingw-w64
LIBS    = -lmingw32 -lopengl32 -lglfw3 -lshell32 -luser32 -lgdi32

RM = del

endif

# -----------------------------------------------------------------------------
# Windows Build with MSVC
# -----------------------------------------------------------------------------
ifeq ($(MSVC), 1)

CXX 		 = cl.exe /std:c++20 /EHsc /nologo /MD
CXX_INC      = /I
CXX_EMIT_OBJ = /c /Fo:
CXX_EMIT_EXE = /Fe:
CXX_LIB      = /link /LIBPATH:

LIB_DIR = .\lib\win\vc2022

LIBS = opengl32.lib glfw3.lib shell32.lib user32.lib gdi32.lib

endif


# -----------------------------------------------------------------------------
# Build variables
# Source files can be added here. 
# Append using the += operator as seen below.
# -----------------------------------------------------------------------------

SRC_FILES  = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)

IMGUI_SRC_FILES  = $(wildcard inc/imgui/*.cpp) $(wildcard inc/imgui/**/*.cpp)
ifeq ($(ZURA_GUI),1)

CXX += -DZURA_GUI
SRC_FILES += $(IMGUI_SRC_FILES)

endif

BIN_DIR   = bin
INC_DIR   = ./inc

OBJ_FILES = $(SRC_FILES:.cpp=.obj)

# -----------------------------------------------------------------------------
# Build Recipes
# -----------------------------------------------------------------------------

# Non-File Targets
.PHONY: zura debug zura_gui zura_gui_debug clean clean_imgui_obj clean_zura \
	imgui_demo

imgui_demo: $(IMGUI_SRC_FILES:.cpp=.obj) 
	$(CXX) $(CXX_INC)$(INC_DIR) $^ $(LIBS) $(CXX_EMIT_EXE)$(BIN_DIR)/$@ $(CXX_LIB)$(LIB_DIR) 

zura: $(OBJ_FILES)
	$(CXX) $(CXX_INC)$(INC_DIR) $^ $(LIBS) $(CXX_EMIT_EXE)$(BIN_DIR)/$@ $(CXX_LIB)$(LIB_DIR) 

%.obj : %.cpp
	$(CXX) $(CXX_INC)$(INC_DIR) $^ $(CXX_EMIT_OBJ)$@

clean_imgui_obj:
	$(RM) $(IMGUI_SRC_FILES:.cpp=.obj) $(IMGUI_DEMO_SRC:.cpp=.obj)

clean_zura: 
	$(RM) $(OBJ_FILES)

clean_bin:
	$(RM) $(BIN_DIR)\*.exe

clean: clean_imgui_obj clean_zura clean_bin

echo_OS: 
	@echo $(OS)

echo_IMGUI: 
	@echo $(IMGUI_DEMO_OBJ)



