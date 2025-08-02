# 🚀 Project Cerberus Chess Engine - Comprehensive Build Instructions

## 📋 Overview

Project Cerberus is an advanced chess engine designed to surpass Stockfish through innovative hybrid AI architecture. This document provides complete build instructions for the MVP v1 implementation as specified in the PRD.

## 🎯 MVP v1 Features (CER-01 to CER-06)

- **CER-01**: Core Engine Framework (bitboards, move generation, game state)
- **CER-02**: Full UCI Protocol Compliance
- **CER-03**: Baseline Alpha-Beta Search (PVS with pruning)
- **CER-04**: Initial Evaluation Function (material, positional, pawn structure)
- **CER-05**: Multi-Threading Support (SMP)
- **CER-06**: Transposition Table

## 🛠️ Prerequisites

### Required Tools

| Tool | Version | Purpose |
|------|---------|---------|
| **CMake** | 3.16+ | Modern build system |
| **C++ Compiler** | C++20 support | GCC 10+, Clang 10+, MSVC 2019+ |
| **Git** | Latest | Version control |

### Optional Tools

| Tool | Purpose |
|------|---------|
| **Google Test** | Unit testing framework |
| **Ninja** | Fast build system |
| **Clang-Format** | Code formatting |
| **Valgrind** | Memory debugging (Linux/macOS) |

## 🖥️ Platform-Specific Setup

### Windows

#### Option A: Visual Studio (Recommended)
```bash
# Install Visual Studio 2022 Community with C++ workload
# Includes MSVC compiler, CMake, and Git

# Verify installation
cmake --version
cl
git --version
```

#### Option B: MinGW-w64 + CMake
```bash
# Download and install:
# 1. CMake from cmake.org
# 2. MinGW-w64 from winlibs.com
# 3. Git from git-scm.com

# Add to PATH:
# C:\mingw64\bin
# C:\Program Files\CMake\bin
# C:\Program Files\Git\bin

# Verify
g++ --version
cmake --version
```

### Linux (Ubuntu/Debian)
```bash
# Install build tools
sudo apt update
sudo apt install -y build-essential cmake git ninja-build

# Install optional tools
sudo apt install -y libgtest-dev clang-format valgrind

# Verify installation
g++ --version
cmake --version
```

### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install build tools
brew install cmake ninja git

# Install optional tools
brew install googletest clang-format

# Verify installation
clang++ --version
cmake --version
```

## 🏗️ Building the Engine

### Method 1: CMake Build (Recommended)

#### Quick Build
```bash
# Clone and build
git clone <repository-url>
cd Chessbot

# Create build directory
mkdir build && cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Run the engine
./cerberus  # Linux/macOS
cerberus.exe  # Windows
```

#### Advanced Build Options
```bash
# Debug build with all warnings
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic"

# Release build with maximum optimization
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"

# Build with tests
cmake .. -DBUILD_TESTS=ON

# Use Ninja for faster builds
cmake .. -GNinja
ninja
```

### Method 2: Direct Compilation (Fallback)

#### Linux/macOS
```bash
# Compile all sources
g++ -std=c++20 -O3 -DNDEBUG -march=native \
    -Iinclude \
    -o cerberus \
    src/main.cpp \
    src/core/*.cpp \
    src/search/*.cpp \
    src/evaluation/*.cpp \
    src/uci/*.cpp \
    -pthread
```

#### Windows (MinGW)
```bash
g++ -std=c++20 -O3 -DNDEBUG -march=native ^
    -Iinclude ^
    -o cerberus.exe ^
    src/main.cpp ^
    src/core/*.cpp ^
    src/search/*.cpp ^
    src/evaluation/*.cpp ^
    src/uci/*.cpp ^
    -pthread
```

## 🧪 Testing

### Unit Tests (Google Test)
```bash
# Build with tests enabled
cmake .. -DBUILD_TESTS=ON
cmake --build . --config Release

# Run tests
ctest --output-on-failure

# Or run directly
./cerberus_tests  # Linux/macOS
cerberus_tests.exe  # Windows
```

### Engine Testing
```bash
# Basic UCI compliance test
echo -e "uci\nisready\nposition startpos\ngo depth 10\nquit" | ./cerberus

# Expected output:
# id name Cerberus
# id author Project Cerberus Team
# uciok
# readyok
# info depth 1 score cp 0 nodes 20 time 1 nps 20000 pv e2e4
# bestmove e2e4
```

### Performance Benchmarking
```bash
# Run performance test
./cerberus bench

# Test with different thread counts
./cerberus bench 16 1  # 1 thread
./cerberus bench 16 4  # 4 threads
./cerberus bench 16 8  # 8 threads
```

## 🎮 Integration with Chess GUIs

### Arena Chess GUI
1. Download from [playwitharena.com](http://www.playwitharena.com/)
2. Install and launch Arena
3. Go to **Engines** → **Install New Engine**
4. Browse to your `cerberus` executable
5. Configure engine options:
   - **Hash**: 128-1024 MB (depending on RAM)
   - **Threads**: Number of CPU cores
   - **MultiPV**: 1 for single best move

### Cute Chess
```bash
# Install Cute Chess
# Linux: sudo apt install cutechess
# macOS: brew install cutechess
# Windows: Download from cutechess.com

# Add engine via GUI:
# Tools → Engine Manager → Add Engine
# Set path to cerberus executable
# Protocol: UCI
```

### Command Line Testing
```bash
# Test engine vs engine match
cutechess-cli \
    -engine cmd=./cerberus name=Cerberus \
    -engine cmd=stockfish name=Stockfish \
    -each tc=10+0.1 \
    -games 100 \
    -pgnout games.pgn
```

## 🚀 Performance Optimization

### Compiler Optimizations
```bash
# Maximum performance build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -flto -fno-exceptions" \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# Profile-guided optimization (advanced)
# 1. Build with profiling
cmake .. -DCMAKE_CXX_FLAGS="-O3 -fprofile-generate"
cmake --build .

# 2. Run training games
./cerberus bench 20

# 3. Rebuild with profile data
cmake .. -DCMAKE_CXX_FLAGS="-O3 -fprofile-use"
cmake --build .
```

### Runtime Configuration
```bash
# Optimal UCI options for different hardware:

# 8-core CPU, 16GB RAM:
setoption name Hash value 2048
setoption name Threads value 7  # Leave 1 core for OS

# 4-core CPU, 8GB RAM:
setoption name Hash value 1024
setoption name Threads value 3

# 16-core CPU, 32GB RAM:
setoption name Hash value 4096
setoption name Threads value 15
```

## 🔧 Development Workflow

### Code Style
```bash
# Format code (if clang-format installed)
find src include -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Check for common issues
cppcheck --enable=all --std=c++20 src/ include/
```

### Debugging
```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Run with debugger
gdb ./cerberus  # Linux
lldb ./cerberus  # macOS

# Memory debugging (Linux)
valgrind --tool=memcheck --leak-check=full ./cerberus
```

### Profiling
```bash
# CPU profiling with perf (Linux)
perf record -g ./cerberus bench 16
perf report

# Memory profiling
valgrind --tool=massif ./cerberus bench 16
ms_print massif.out.*
```

## 📦 Distribution

### Creating Release Packages
```bash
# Build release
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Create packages
cpack

# This creates:
# - cerberus-1.0.0-Linux.tar.gz (Linux)
# - cerberus-1.0.0-Darwin.tar.gz (macOS)  
# - cerberus-1.0.0-win64.zip (Windows)
```

## 🐛 Troubleshooting

### Common Build Issues

#### "CMake not found"
```bash
# Install CMake
# Ubuntu: sudo apt install cmake
# macOS: brew install cmake
# Windows: Download from cmake.org
```

#### "C++20 not supported"
```bash
# Update compiler
# GCC: sudo apt install gcc-10 g++-10
# Clang: sudo apt install clang-10
# MSVC: Update Visual Studio
```

#### "Threading errors"
```bash
# Ensure pthread support
# Linux: Install build-essential
# Windows: Use MinGW-w64 or MSVC
```

### Runtime Issues

#### "Engine crashes on startup"
```bash
# Check dependencies
ldd ./cerberus  # Linux
otool -L ./cerberus  # macOS

# Run with debug symbols
gdb --args ./cerberus
(gdb) run
(gdb) bt  # If crash occurs
```

#### "Poor performance"
```bash
# Check build type
./cerberus --version

# Verify optimizations enabled
objdump -d cerberus | grep -A5 "main>"

# Monitor resource usage
top -p $(pgrep cerberus)
```

## 📊 Benchmarking & Validation

### Standard Test Suites
```bash
# WAC (Win At Chess) - Tactical test suite
./cerberus < wac.epd

# STS (Strategic Test Suite) - Positional test suite  
./cerberus < sts.epd

# Perft (Move generation verification)
./cerberus perft 6
```

### ELO Testing
```bash
# Self-play tournament
cutechess-cli \
    -engine cmd=./cerberus name="Cerberus v1.0" \
    -engine cmd=./cerberus_old name="Cerberus v0.9" \
    -each tc=60+0.6 \
    -games 1000 \
    -concurrency 4 \
    -pgnout tournament.pgn
```

## 🎯 Success Criteria

Your build is successful when:

- ✅ Engine compiles without errors
- ✅ All unit tests pass
- ✅ UCI protocol compliance verified
- ✅ Can play complete games in GUI
- ✅ Achieves target ELO (2000+ for MVP)
- ✅ Multi-threading scales properly
- ✅ Memory usage is reasonable (<2GB)

## 📚 Additional Resources

- **UCI Protocol**: [shredderchess.com/chess-info/features/uci-universal-chess-interface.html](http://www.shredderchess.com/chess-info/features/uci-universal-chess-interface.html)
- **Chess Programming Wiki**: [chessprogramming.org](https://www.chessprogramming.org/)
- **Engine Testing**: [computerchess.org.uk](http://www.computerchess.org.uk/)
- **Stockfish Source**: [github.com/official-stockfish/Stockfish](https://github.com/official-stockfish/Stockfish)

---

## 🏆 Next Steps

After successful MVP v1 build:

1. **Integrate CerberusNet** (Neural network evaluation)
2. **Implement self-play training** (Reinforcement learning)
3. **Optimize search algorithms** (Dynamic pruning)
4. **Add analysis features** (Move explanations)
5. **Deploy cloud version** (Distributed computing)

**Your Project Cerberus engine is ready to challenge the chess world!** 🚀

---

*For technical support, please refer to the project documentation or open an issue on GitHub.*