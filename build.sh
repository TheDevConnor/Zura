#!/bin/bash

# Variables
CXX="g++"
CXXFLAGS="-O2 -std=c++17 -Wall -Wextra"
CXXFLAGS_DEBUG="-O0 -g -std=c++11 -Wall -ggdb3"
VALGRIND="valgrind"
BIN_PATH="bin"
OBJ_PATH="obj"
BIN_DEBUG_PATH="bin/debug"
SRC_PATH="src"
COMPILER_PATH="src/compiler"
DEBUG_PATH="src/debug"
GC_PATH="src/garbage_collector"
MEMORY_PATH="src/memory"
SRC_PARSER_PATH="src/parser"
TYPE_PATH="src/types"
VM_PATH="src/vm"

# Helper Functions
print_progress_bar() {
    local iteration=$1
    local total=$2
    local prefix=$3
    local suffix=$4
    local decimals=$5
    local length=$6
    local fill=$7

    local percent=$(awk "BEGIN { pc=100*${iteration}/${total}; i=int(pc); print (pc-i<0.5)?i:i+1 }")
    local filled_length=$(awk "BEGIN { fl=${length}*${iteration}/${total}; i=int(fl); print (fl-i<0.5)?i:i+1 }")
    local bar=$(printf "%-${filled_length}s" "${fill}")
    bar=${bar// /"${fill}"}
    local spaces=$(printf "%-$((length-filled_length))s")

    echo -ne "\r${prefix} [${bar}${spaces}] ${percent}% ${suffix}"
}

clean() {
    echo "Cleaning..."
    rm -f "$BIN_PATH"/*
    rm -f "$OBJ_PATH"/*
    echo "Cleaned"
}

linux() {
    mkdir -p "$BIN_PATH" # Create the bin directory if it doesn't exist
    mkdir -p "$OBJ_PATH" # Create the obj directory if it doesn't exist
    # For Linux, use backslashes in the paths
    TARGET="$BIN_PATH/zura"

    # Collect all source files and count them
    SOURCE_FILES=$(find "$SRC_PATH" -name "*.cpp")
    total_files=$(echo "$SOURCE_FILES" | wc -l)
    current_file=0

    # Compile each source file to object files
    for file in $SOURCE_FILES; do
        current_file=$((current_file + 1))
        print_progress_bar "$current_file" "$total_files" "Building (Linux)" "zura" 0 30 "#"
        $CXX -c "$file" -o "$OBJ_PATH/${file##*/}.o" $CXXFLAGS
    done

    # Collect all object files and specify them for linking
    OBJECT_FILES=$(find "$OBJ_PATH" -name "*.o")
    $CXX -o "$TARGET" $OBJECT_FILES $CXXFLAGS

    echo -e "\nBuild for Linux completed."
}

windows() {
    mkdir -p "$BIN_PATH" # Create the bin directory if it doesn't exist
    mkdir -p "$OBJ_PATH" # Create the obj directory if it doesn't exist
    # For Windows, use backslashes in the paths
    TARGET="$BIN_PATH\\zura.exe"

    # Collect all source files and count them
    SOURCE_FILES=$(find "$SRC_PATH" -name "*.cpp")
    total_files=$(echo "$SOURCE_FILES" | wc -l)
    current_file=0

    # Compile each source file to object files
    for file in $SOURCE_FILES; do
        current_file=$((current_file + 1))
        print_progress_bar "$current_file" "$total_files" "Building (Windows)" "zura.exe" 0 30 "#"
        $CXX -c "$file" -o "$OBJ_PATH\\${file##*/}.o" $CXXFLAGS
    done

    # Collect all object files and specify them for linking
    OBJECT_FILES=$(find "$OBJ_PATH" -name "*.o")
    $CXX -o "$TARGET" $OBJECT_FILES $CXXFLAGS

    echo -e "\nBuild for Windows completed."
}

# Main script
if [ "$1" = "clean" ]; then
    clean
elif [ "$1" = "linux" ]; then
    linux
elif [ "$1" = "windows" ]; then
    windows
else
    echo "Usage: $0 [clean | linux | windows ]"
    exit 1
fi
