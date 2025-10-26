# CHIP-8 Emulator

A modern CHIP-8 emulator written in C++23 with SDL3 for graphics and audio.

## Features

- **Complete CHIP-8 instruction set implementation**
- **SDL3-based rendering**
- **Audio support** with square wave beep
- **Configurable execution speed** (instructions per frame)

## Requirements

- **CMake** 3.20 or higher
- **C++23-compatible compiler** (GCC 13+, Clang 16+, or MSVC 2022+)
- **Git** (for submodules)

## Building

### 1. Clone the repository with submodules

```bash
git clone --recursive git@github.com:d-3nnis/d3nnis-emus.git
```

If you cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

### 2. Configure and build

**Debug build:**
```bash
./build.bash build
```

**Release build:**
```bash
./build.bash release
```

## Usage

```bash
./build.bash run-release <path to rom> <instructions_per_frame>
```

## Controls

The CHIP-8 keypad is mapped to your keyboard as follows:

```
CHIP-8 Keypad:     Keyboard:
┌─┬─┬─┬─┐          ┌─┬─┬─┬─┐
│1│2│3│C│          │1│2│3│4│
├─┼─┼─┼─┤          ├─┼─┼─┼─┤
│4│5│6│D│          │Q│W│E│R│
├─┼─┼─┼─┤          ├─┼─┼─┼─┤
│7│8│9│E│          │A│S│D│F│
├─┼─┼─┼─┤          ├─┼─┼─┼─┤
│A│0│B│F│          │Z│X│C│V│
└─┴─┴─┴─┘          └─┴─┴─┴─┘
```


## Technical Details

### Display
- **Resolution:** 64x32 pixels
- **Refresh rate:** 60 Hz
- **Rendering:** Monochrome (white on black)

### Audio
- **Waveform:** Square wave
- **Frequency:** 440 Hz (A4)
- **Sample rate:** 44100 Hz

### Memory Layout
- **Total memory:** 4096 bytes
- **Font sprites:** 0x000-0x07F
- **Program start:** 0x200
- **Display buffer:** 0xF00-0xFFF

### Timers
- **Delay timer:** Decrements at 60 Hz
- **Sound timer:** Decrements at 60 Hz (beep when >0)

## Known Issues

- Some ROMs may require specific instruction-per-frame tuning for optimal speed
- The shift instructions (8XY6, 8XYE) and load/store instructions (FX55, FX65) use the modern CHIP-8 behavior (there are conflicting specifications)

## Resources

- [CHIP-8 Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [Columbia CS CHIP-8 Design Spec](https://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf)

## License

This project is licensed under the terms of the MIT license
