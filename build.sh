#!/bin/bash

# ===================
# 🔧 Configuration
# ===================
CXX=g++
INCLUDES="-Iheaders"
SOURCES="main.cpp global.cpp util.cpp"
OUTPUT="scaf"

# ===================
# 🧠 Build Mode Logic
# ===================
MODE=${1:-release}  # Default to release if no argument

if [ "$MODE" == "debug" ]; then
    echo "[*] Building in DEBUG mode..."
    CXXFLAGS="-std=c++20 -Wall -Wextra -pedantic -g -fsanitize=address -DDEBUG"
elif [ "$MODE" == "release" ]; then
    echo "[*] Building in RELEASE mode..."
    CXXFLAGS="-std=c++20 -Wall -Wextra -pedantic -O2"
else
    echo "[!] Unknown build mode: $MODE"
    echo "Usage: ./build.sh [debug|release]"
    exit 1
fi

# ===================
# 🚀 Compile
# ===================
$CXX $CXXFLAGS $INCLUDES $SOURCES -o $OUTPUT

if [ $? -eq 0 ]; then
    echo "[✓] Build successful: $OUTPUT"
else
    echo "[✗] Build failed."
    exit 1
fi
