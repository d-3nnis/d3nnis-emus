.PHONY: run debug release clean watch

# Default target - build and run
run: debug
	./build/debug/chip8_emulator ./test_roms/life.ch8

# Quick development cycle
dev: clean debug run

# Build debug
debug:
	cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug -j
	ln -sf build/debug/compile_commands.json .

# Auto-rebuild on file changes (requires entr)
watch:
	find src -name "*.cpp" -o -name "*.h" | entr -r make run

# Run with ROM file
rom: debug
	./build/debug/chip8_emulator roms/pong.ch8

clean:
	rm -rf build/
