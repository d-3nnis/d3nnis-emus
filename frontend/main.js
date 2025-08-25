// CHIP-8 Web Emulator Frontend

class Chip8Frontend {
    constructor() {
        this.wasm = null;
        this.emulator = null;
        this.isRunning = false;
        this.animationId = null;
        this.speed = 5;
        
        this.canvas = document.getElementById('display');
        this.ctx = this.canvas.getContext('2d');
        this.ctx.imageSmoothingEnabled = false;
        
        // Initialize display
        this.clearDisplay();
        
        // Set up event listeners
        this.setupEventListeners();
        
        // Load WASM module
        this.loadWasm();
    }

    async loadWasm() {
        try {
            // TODO: Load WASM module when backend is built
            // this.wasm = await import('./pkg/chip8_emulator.js');
            // await this.wasm.default();
            // this.emulator = new this.wasm.Chip8();
            
            console.log('WASM loading placeholder - build backend first');
            
            // For now, create a mock emulator for testing
            this.emulator = new MockChip8();
            
        } catch (error) {
            console.error('Failed to load WASM:', error);
            this.showError('Failed to load emulator. Make sure to build the backend first.');
        }
    }

    setupEventListeners() {
        // File upload
        const romUpload = document.getElementById('rom-upload');
        romUpload.addEventListener('change', (e) => this.handleRomUpload(e));

        // Control buttons
        document.getElementById('start-btn').addEventListener('click', () => this.start());
        document.getElementById('pause-btn').addEventListener('click', () => this.pause());
        document.getElementById('reset-btn').addEventListener('click', () => this.reset());

        // Speed control
        const speedSlider = document.getElementById('speed-slider');
        const speedValue = document.getElementById('speed-value');
        speedSlider.addEventListener('input', (e) => {
            this.speed = parseInt(e.target.value);
            speedValue.textContent = this.speed;
        });

        // Keyboard input
        document.addEventListener('keydown', (e) => this.handleKeyDown(e));
        document.addEventListener('keyup', (e) => this.handleKeyUp(e));

        // Virtual keypad
        document.querySelectorAll('.key').forEach(key => {
            key.addEventListener('mousedown', (e) => {
                const keyValue = e.target.dataset.key;
                this.setKey(keyValue, true);
                e.target.classList.add('active');
            });
            
            key.addEventListener('mouseup', (e) => {
                const keyValue = e.target.dataset.key;
                this.setKey(keyValue, false);
                e.target.classList.remove('active');
            });
            
            key.addEventListener('mouseleave', (e) => {
                const keyValue = e.target.dataset.key;
                this.setKey(keyValue, false);
                e.target.classList.remove('active');
            });
        });
    }

    handleRomUpload(event) {
        const file = event.target.files[0];
        if (!file) return;

        const reader = new FileReader();
        reader.onload = (e) => {
            const romData = new Uint8Array(e.target.result);
            this.loadRom(romData, file.name);
        };
        reader.readAsArrayBuffer(file);
    }

    loadRom(romData, filename) {
        if (!this.emulator) {
            this.showError('Emulator not loaded yet');
            return;
        }

        try {
            this.emulator.load_rom(romData);
            document.getElementById('rom-name').textContent = filename;
            
            // Enable controls
            document.getElementById('start-btn').disabled = false;
            document.getElementById('reset-btn').disabled = false;
            
            console.log(`Loaded ROM: ${filename} (${romData.length} bytes)`);
        } catch (error) {
            console.error('Failed to load ROM:', error);
            this.showError('Failed to load ROM file');
        }
    }

    start() {
        if (!this.emulator) return;
        
        this.isRunning = true;
        document.getElementById('start-btn').disabled = true;
        document.getElementById('pause-btn').disabled = false;
        
        this.gameLoop();
        console.log('Emulator started');
    }

    pause() {
        this.isRunning = false;
        if (this.animationId) {
            cancelAnimationFrame(this.animationId);
        }
        
        document.getElementById('start-btn').disabled = false;
        document.getElementById('pause-btn').disabled = true;
        
        console.log('Emulator paused');
    }

    reset() {
        this.pause();
        
        if (this.emulator) {
            // TODO: Implement reset in WASM
            // this.emulator.reset();
            this.clearDisplay();
        }
        
        console.log('Emulator reset');
    }

    gameLoop() {
        if (!this.isRunning) return;

        // Run multiple cycles per frame based on speed
        const cyclesPerFrame = this.speed * 2;
        
        for (let i = 0; i < cyclesPerFrame; i++) {
            if (this.emulator) {
                this.emulator.step();
            }
        }

        // Update display
        this.updateDisplay();

        // Schedule next frame
        this.animationId = requestAnimationFrame(() => this.gameLoop());
    }

    updateDisplay() {
        if (!this.emulator) return;

        // TODO: Get display buffer from WASM
        // const displayBuffer = this.emulator.get_display_buffer();
        
        // For now, use mock display
        const displayBuffer = this.emulator.getDisplay();
        
        // Clear canvas
        this.ctx.fillStyle = '#000000';
        this.ctx.fillRect(0, 0, 640, 320);

        // Draw pixels (10x scale: 64x32 -> 640x320)
        this.ctx.fillStyle = '#FFFFFF';
        for (let y = 0; y < 32; y++) {
            for (let x = 0; x < 64; x++) {
                const index = y * 64 + x;
                if (displayBuffer[index]) {
                    this.ctx.fillRect(x * 10, y * 10, 10, 10);
                }
            }
        }
    }

    clearDisplay() {
        this.ctx.fillStyle = '#000000';
        this.ctx.fillRect(0, 0, 640, 320);
    }

    // Keyboard mapping: CHIP-8 keys to keyboard keys
    getKeyMapping() {
        return {
            '1': 0x1, '2': 0x2, '3': 0x3, '4': 0xC,
            'q': 0x4, 'w': 0x5, 'e': 0x6, 'r': 0xD,
            'a': 0x7, 's': 0x8, 'd': 0x9, 'f': 0xE,
            'z': 0xA, 'x': 0x0, 'c': 0xB, 'v': 0xF
        };
    }

    handleKeyDown(event) {
        const mapping = this.getKeyMapping();
        const chip8Key = mapping[event.key.toLowerCase()];
        
        if (chip8Key !== undefined) {
            event.preventDefault();
            this.setKey(chip8Key, true);
            
            // Highlight virtual key
            const virtualKey = document.querySelector(`[data-key="${chip8Key.toString(16).toUpperCase()}"]`);
            if (virtualKey) {
                virtualKey.classList.add('active');
            }
        }
    }

    handleKeyUp(event) {
        const mapping = this.getKeyMapping();
        const chip8Key = mapping[event.key.toLowerCase()];
        
        if (chip8Key !== undefined) {
            event.preventDefault();
            this.setKey(chip8Key, false);
            
            // Remove highlight from virtual key
            const virtualKey = document.querySelector(`[data-key="${chip8Key.toString(16).toUpperCase()}"]`);
            if (virtualKey) {
                virtualKey.classList.remove('active');
            }
        }
    }

    setKey(key, pressed) {
        if (this.emulator) {
            // Convert hex string to number if needed
            const keyNum = typeof key === 'string' ? parseInt(key, 16) : key;
            // TODO: Implement in WASM
            // this.emulator.set_key(keyNum, pressed);
            this.emulator.setKey(keyNum, pressed);
        }
    }

    showError(message) {
        // Simple error display - could be improved with a modal
        alert(`Error: ${message}`);
    }
}

// Mock CHIP-8 emulator for testing without WASM
class MockChip8 {
    constructor() {
        this.display = new Uint8Array(64 * 32);
        this.keys = new Array(16).fill(false);
        this.romLoaded = false;
        
        // Create a simple test pattern
        this.createTestPattern();
    }

    load_rom(data) {
        this.romLoaded = true;
        console.log(`Mock: Loaded ${data.length} bytes`);
        this.createTestPattern();
    }

    step() {
        // Mock CPU step - just animate the test pattern
        if (this.romLoaded) {
            this.animateTestPattern();
        }
    }

    getDisplay() {
        return this.display;
    }

    setKey(key, pressed) {
        if (key >= 0 && key < 16) {
            this.keys[key] = pressed;
            console.log(`Mock: Key ${key.toString(16)} ${pressed ? 'pressed' : 'released'}`);
        }
    }

    createTestPattern() {
        // Create a simple CHIP-8 logo pattern
        this.display.fill(0);
        
        // Draw "CHIP-8" in a simple pixel font
        const pattern = [
            "  ####  #   # # ####        ##  ",
            " #      #   # # #   #      #  # ",
            " #      ##### # ####   ### #  # ",
            " #      #   # # #          #  # ",
            "  ####  #   # # #           ##  "
        ];
        
        for (let y = 0; y < pattern.length; y++) {
            for (let x = 0; x < pattern[y].length; x++) {
                if (pattern[y][x] === '#') {
                    const displayX = x + 16; // Center horizontally
                    const displayY = y + 12; // Center vertically
                    if (displayX < 64 && displayY < 32) {
                        this.display[displayY * 64 + displayX] = 1;
                    }
                }
            }
        }
    }

    animateTestPattern() {
        // Simple animation - just blink a border
        const frame = Math.floor(Date.now() / 500) % 2;
        
        // Draw border
        for (let x = 0; x < 64; x++) {
            this.display[x] = frame; // Top
            this.display[31 * 64 + x] = frame; // Bottom
        }
        
        for (let y = 0; y < 32; y++) {
            this.display[y * 64] = frame; // Left
            this.display[y * 64 + 63] = frame; // Right
        }
    }
}

// Initialize the emulator when the page loads
document.addEventListener('DOMContentLoaded', () => {
    new Chip8Frontend();
});
