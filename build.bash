#!/bin/bash
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper function
print_usage() {
    cat << EOF
Usage: ./build.bash <command> [args]

Commands:
  build               Build debug version
  release             Build release version
  run <rom> [speed]   Run debug build (default speed: 500)
  run-release <rom> [speed]  Run release build
  clean               Remove build directory
  fmt                 Format code with clang-format
  help                Show this help

Examples:
  ./build.bash build
  ./build.bash run test_roms/life.ch8 500
  ./build.bash run test_roms/pong.ch8 10
  ./build.bash release
  ./build.bash run-release test_roms/life.ch8 1000
EOF
}

# Commands
cmd_build() {
    echo -e "${BLUE}Building debug...${NC}"
    cmake --preset debug
    cmake --build --preset debug
    ln -sf build/debug/compile_commands.json .
    echo -e "${GREEN}û Debug build complete${NC}"
}

cmd_release() {
    echo -e "${BLUE}Building release...${NC}"
    cmake --preset release
    cmake --build --preset release
    echo -e "${GREEN}û Release build complete${NC}"
}

cmd_run() {
    local rom="${1}"
    local speed="${2:-500}"
    
    if [ -z "$rom" ]; then
        echo -e "${RED}Error: ROM file required${NC}"
        echo "Usage: ./build.bash run <rom> [speed]"
        exit 1
    fi
    
    if [ ! -f "$rom" ]; then
        echo -e "${RED}Error: ROM file not found: $rom${NC}"
        exit 1
    fi
    
    echo -e "${BLUE}Running $rom (speed: $speed)${NC}"
    ./build/debug/chip8_emulator "$rom" "$speed"
}

cmd_run_release() {
    local rom="${1}"
    local speed="${2:-500}"
    
    if [ -z "$rom" ]; then
        echo -e "${RED}Error: ROM file required${NC}"
        echo "Usage: ./build.bash run-release <rom> [speed]"
        exit 1
    fi
    
    if [ ! -f "$rom" ]; then
        echo -e "${RED}Error: ROM file not found: $rom${NC}"
        exit 1
    fi
    
    echo -e "${BLUE}Running $rom (speed: $speed)${NC}"
    ./build/release/chip8_emulator "$rom" "$speed"
}

cmd_clean() {
    echo -e "${BLUE}Cleaning build artifacts...${NC}"
    rm -rf build/ compile_commands.json
    echo -e "${GREEN}û Clean complete${NC}"
}

cmd_fmt() {
    echo -e "${BLUE}Formatting code...${NC}"
    find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
    echo -e "${GREEN}û Format complete${NC}"
}

# Main
case "${1:-}" in
    build)
        cmd_build
        ;;
    release)
        cmd_release
        ;;
    run)
        shift
        cmd_run "$@"
        ;;
    run-release)
        shift
        cmd_run_release "$@"
        ;;
    clean)
        cmd_clean
        ;;
    fmt)
        cmd_fmt
        ;;
    help|--help|-h)
        print_usage
        ;;
    *)
        echo -e "${RED}Error: Unknown command '${1:-}'${NC}"
        echo ""
        print_usage
        exit 1
        ;;
esac
