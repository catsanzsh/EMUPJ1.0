;;;

// Write a byte to memory
void write_byte(uint32_t address, uint8_t value) {
    if (address >= MEMORY_SIZE) {
        fprintf(stderr, "Memory write out of bounds: 0x%08X\n", address);
        return;
    }
    memory[address] = value;
}

// Read a word (4 bytes) from memory
uint32_t read_word(uint32_t address) {
    if (address + 3 >= MEMORY_SIZE) {
        fprintf(stderr, "Memory read out of bounds: 0x%08X\n", address);
        return 0;
    }
    return (memory[address] << 24) | (memory[address + 1] << 16) | (memory[address + 2] << 8) | memory[address + 3];
}

// Write a word (4 bytes) to memory
void write_word(uint32_t address, uint32_t value) {
    if (address + 3 >= MEMORY_SIZE) {
        fprintf(stderr, "Memory write out of bounds: 0x%08X\n", address);
        return;
    }
    memory[address] = (value >> 24) & 0xFF;
    memory[address + 1] = (value >> 16) & 0xFF;
    memory[address + 2] = (value >> 8) & 0xFF;
    memory[address + 3] = value & 0xFF;
}

// Shutdown Memory
void shutdown_memory() {
    if (memory) {
        free(memory);
        memory = NULL;
    }
}

// ------------------------
// ROM Loader
// ------------------------
bool load_rom(const char *filename) {
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        perror("Failed to open ROM file");
        return false;
    }

    // Get ROM size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    if (rom_size > MEMORY_SIZE) {
        fprintf(stderr, "ROM size (%ld bytes) exceeds memory size (%d bytes).\n", rom_size, MEMORY_SIZE);
        fclose(rom);
        return false;
    }

    size_t bytes_read = fread(memory, 1, rom_size, rom);
    if (bytes_read != rom_size) {
        fprintf(stderr, "Failed to read the complete ROM file.\n");
        fclose(rom);
        return false;
    }

    fclose(rom);
    printf("ROM loaded successfully. Size: %ld bytes.\n", rom_size);
    return true;
}

// ------------------------
// CPU Emulation
// ------------------------
typedef struct {
    uint32_t registers[32];
    uint32_t pc; // Program Counter
} CPUState;

CPUState cpu;

// Initialize CPU
bool init_cpu() {
    memset(&cpu, 0, sizeof(CPUState));
    cpu.pc = 0x80000000; // Typical N64 start address
    // Initialize stack pointer (example value)
    cpu.registers[29] = 0x807FFFE0;
    return true;
}

// Simplified CPU Step (Fetch-Decode-Execute)
bool cpu_step() {
    // Fetch
    uint32_t instr = read_word(cpu.pc);
    // For demonstration, just print the instruction and increment PC
    printf("PC: 0x%08X | Instruction: 0x%08X\n", cpu.pc, instr);
    // Increment PC
    cpu.pc += 4;

    // Decode and Execute
    // NOTE: This is where you would implement the MIPS instruction decoding and execution.
    // For this skeleton, we'll stop after fetching.

    // To prevent infinite loop in this skeleton
    return false; // Return false to indicate stopping the emulation loop
}

// Shutdown CPU
void shutdown_cpu() {
    // Clean up CPU state if necessary
}

// ------------------------
// GPU Emulation (Placeholder)
// ------------------------
bool init_gpu() {
    // Initialize GPU components (RDP, RSP)
    printf("GPU initialized (placeholder).\n");
    return true;
}

bool gpu_step() {
    // Handle GPU tasks like rendering
    // Placeholder implementation
    return true;
}

void shutdown_gpu() {
    // Clean up GPU resources
    printf("GPU shutdown (placeholder).\n");
}

// ------------------------
// Main Function
// ------------------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <Super_Mario_64_ROM.z64>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize Memory
    if (!init_memory()) {
        return EXIT_FAILURE;
    }

    // Load ROM
    if (!load_rom(argv[1])) {
        shutdown_memory();
        return EXIT_FAILURE;
    }

    // Initialize CPU
    if (!init_cpu()) {
        shutdown_memory();
        return EXIT_FAILURE;
    }

    // Initialize GPU
    if (!init_gpu()) {
        shutdown_cpu();
        shutdown_memory();
        return EXIT_FAILURE;
    }

    // Emulation Loop
    bool running = true;
    while (running) {
        // CPU Step
        running = cpu_step();

        // GPU Step
        gpu_step();

        // Handle Events (Input, Display, etc.)
        // Placeholder: No event handling in this skeleton

        // For demonstration, we'll run only one CPU step
    }

    // Shutdown Components
    shutdown_gpu();
    shutdown_cpu();
    shutdown_memory();

    printf("Emulation terminated.\n");
    return EXIT_SUCCESS;
}
