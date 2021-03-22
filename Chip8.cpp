#include "Chip8.h"



unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::initialize() {
	opcode = 0;		// Reset current opcode
	I = 0;			// Reset index register
	pc = 0x200;		// Program counter starts at 0x200
	sp = 0;			// Reset stack pointer
	// Clear memory
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}
	// Clear registers V0-VF
	for (int i = 0; i < 16; i++) {
		V[i] = 0;
	}
	// Clear display
	for (int i = 0; i < 64*32; i++) {
		gfx[i] = 0;
	}
	// Clear stack
	for (int i = 0; i < 16; i++) {
		stack[i] = 0;
	}
	
	for (int i = 0; i < 16; i++) {
		key[i] = 0;
	}
	
	// Load fontset
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	drawFlag = true;
};

void Chip8::loadGame(const char* filename) {

	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[512 + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}

void Chip8::setKeys() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				exit(0);
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_1:
						key[0] = 1;
						break;
					case SDLK_2:
						key[1] = 1;
						break;
					case SDLK_3:
						key[2] = 1;
						break;
					case SDLK_4:
						key[3] = 1;
						break;
					case SDLK_q:
						key[4] = 1;
						break;
					case SDLK_w:
						key[5] = 1;
						break;
					case SDLK_e:
						key[6] = 1;
						break;
					case SDLK_r:
						key[7] = 1;
						break;
					case SDLK_a:
						key[8] = 1;
						break;
					case SDLK_s:
						key[9] = 1;
						break;
					case SDLK_d:
						key[10] = 1;
						break;
					case SDLK_f:
						key[11] = 1;
						break;
					case SDLK_z:
						key[12] = 1;
						break;
					case SDLK_x:
						key[13] = 1;
						break;
					case SDLK_c:
						key[14] = 1;
						break;
					case SDLK_v:
						key[15] = 1;
						break;
					case SDLK_ESCAPE:
						exit(0);
						break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_1:
					key[0] = 0;
					break;
				case SDLK_2:
					key[1] = 0;
					break;
				case SDLK_3:
					key[2] = 0;
					break;
				case SDLK_4:
					key[3] = 0;
					break;
				case SDLK_q:
					key[4] = 0;
					break;
				case SDLK_w:
					key[5] = 0;
					break;
				case SDLK_e:
					key[6] = 0;
					break;
				case SDLK_r:
					key[7] = 0;
					break;
				case SDLK_a:
					key[8] = 0;
					break;
				case SDLK_s:
					key[9] = 0;
					break;
				case SDLK_d:
					key[10] = 0;
					break;
				case SDLK_f:
					key[11] = 0;
					break;
				case SDLK_z:
					key[12] = 0;
					break;
				case SDLK_x:
					key[13] = 0;
					break;
				case SDLK_c:
					key[14] = 0;
					break;
				case SDLK_v:
					key[15] = 0;
					break;
				default:
					break;
				}
				break;
			default:
				break;
		}
	}
}

void Chip8::disassemble(unsigned short opcode, int x, int y, int z) {

	printf("0x%X: ", opcode);
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x00E0: Clears the screen
			printf("CLS\n");
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			printf("RET\n");
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;

	case 0x1000: // 0x1NNN: Jumps to address NNN
		printf("JMP 0x%X\n", pc);
		break;

	case 0x2000: // 0x2NNN: Calls subroutine at NNN.
		printf("CALL 0x%X\n", opcode & 0x0FFF);
		break;

	case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN
		printf("SE V[%X], 0x%X\n", x, z);
		break;

	case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN
		printf("SNE V[%X], 0x%X\n", x, z);
		break;
	case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
		printf("SE V[%X], V[%X]\n", x, y);
		break;

	case 0x6000: // 0x6XNN: Sets VX to NN.
		printf("LD V[%X], 0x%X\n", x, z);
		break;

	case 0x7000: // 0x7XNN: Adds NN to VX.
		printf("ADD V[%X], 0x%X\n", x, z);
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x8XY0: Sets VX to the value of VY
			printf("LD V[%X], V[%X]\n", x, y);
			break;

		case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
			printf("OR V[%X], V[%X]\n", x, y);
			break;

		case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
			printf("AND V[%X], V[%X]\n", x, y);
			break;

		case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
			printf("XOR V[%X], V[%X]\n", x, y);
			break;

		case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't	
			printf("ADD V[%X], V[%X]\n", x, y);
			break;

		case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			printf("SUB V[%X], V[%X]\n", x, y);
			break;

		case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
			printf("SHR V[%X]\n", x);
			break;

		case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			printf("SUBN V[%X], V[%X]\n", x, y);
			break;

		case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
			printf("SHL V[%X], V[%X]\n", x, y);
			break;

		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
		}
		break;

	case 0x9000: // 0x9XY0: Skips the next instruction if VX doesn't equal VY
		printf("SNE V[%X], V[%X]\n", x, y);
		break;

	case 0xA000: // ANNN: Sets I to the address NNN
		printf("LD I, 0x%X\n", opcode & 0x0FFF);
		break;

	case 0xB000: // BNNN: Jumps to the address NNN plus V0
		printf("JP V0, 0x%X\n", opcode & 0x0FFF);
		break;

	case 0xC000: // CXNN: Sets VX to a random number and NN
		printf("RND V[%X], 0x%X\n", x, z);
		break;

	case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
				 // Each row of 8 pixels is read as bit-coded starting from memory location I; 
				 // I value doesn't change after the execution of this instruction. 
				 // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
				 // and to 0 if that doesn't happen
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;
		printf("DRW V[%X]=%X, V[%X]=%X, %X \n", x, V[x], y, V[y], height);
	}
	break;

	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
			printf("SKP V[%X]\n", x);
			break;
		case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
			printf("SKNP V[%X]\n", x);
			break;
		default:
			printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
		}
		break;

	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: // FX07: Sets VX to the value of the delay timer
			printf("LD V[%X], DT\n", x);
			break;
		case 0x000A: // FX0A: A key press is awaited, and then stored in VX	
			printf("LD V[%X], K\n", x);
			break;
		case 0x0015: // FX15: Sets the delay timer to VX
			printf("LD DT, V[%X]\n", x);
			break;
		case 0x0018: // FX18: Sets the sound timer to VX
			printf("LD ST, V[%X]\n", x);
			break;
		case 0x001E: // FX1E: Adds VX to I
			printf("ADD I, V[%X]\n", x);
			break;
		case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			printf("LD F, V[%X]\n", x);
			break;
		case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
			printf("LD B, V[%X]\n", x);
			break;
		case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
			printf("LD I, V[%X]\n", x);
			break;
		case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I				
			printf("LD V[%X], I\n", x);
			break;
		default:
			printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
		}
		break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
	}
	
}



void Chip8::emulateCycle()
{
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	//printf("0x%X: ", opcode);
	int x;
	int y;
	int z;

	x = (opcode & 0x0F00) >> 8;
	y = (opcode & 0x00F0) >> 4;
	z = (opcode & 0x00FF);

	if (debug == true) {
		disassemble(opcode,x,y,z);
	}
	

	// Process opcode
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x00E0: Clears the screen
			for (int i = 0; i < 2048; ++i) {
				gfx[i] = 0x0;
			}
			drawFlag = true;
			pc += 2;
			break;

		case 0x000E: // 0x00EE: Returns from subroutine
			--sp;
			pc = stack[sp];				
			pc += 2;
			break;

		}
		break;

	case 0x1000: // 0x1NNN: Jumps to address NNN
		pc = opcode & 0x0FFF;
		break;

	case 0x2000: // 0x2NNN: Calls subroutine at NNN.
		stack[sp] = pc;			// Store current address in stack
		++sp;					// Increment stack pointer
		pc = opcode & 0x0FFF;	// Set the program counter to the address at NNN
		break;

	case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN
		if (V[x] == z) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN
		if (V[x] != z) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
		if (V[x] == V[y]) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0x6000: // 0x6XNN: Sets VX to NN.
		V[x] = z;
		pc += 2;
		break;

	case 0x7000: // 0x7XNN: Adds NN to VX.
		V[x] += z;
		pc += 2;
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 0x8XY0: Sets VX to the value of VY
			V[x] = V[y];
			pc += 2;
			break;

		case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
			V[x] |= V[y];
			pc += 2;
			break;

		case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
			V[x] &= V[y];
			pc += 2;
			break;

		case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
			V[x] ^= V[y];
			pc += 2;
			break;

		case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
			if (V[y] > (0xFF - V[x])) {
				V[0xF] = 1;
			} else {
				V[0xF] = 0;
			}
			V[x] += V[y];
			pc += 2;
			break;

		case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			if (V[y] > V[x]) {
				V[0xF] = 0;
			} else {
				V[0xF] = 1;
			}
			V[x] -= V[y];
			pc += 2;
			break;

		case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
			V[0xF] = V[x] & 0x1;
			V[x] >>= 1;
			pc += 2;
			break;

		case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			if (V[x] > V[y]) {
				V[0xF] = 0;
			} else {
				V[0xF] = 1;
			}
			V[x] = V[y] - V[x];
			pc += 2;
			break;

		case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
			V[0xF] = V[x] >> 7;
			V[x] <<= 1;
			pc += 2;
			break;

		}
		break;

	case 0x9000: // 0x9XY0: Skips the next instruction if VX doesn't equal VY
		if (V[x] != V[y]) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0xA000: // ANNN: Sets I to the address NNN
		I = opcode & 0x0FFF;
		pc += 2;
		break;

	case 0xB000: // BNNN: Jumps to the address NNN plus V0
		pc = (opcode & 0x0FFF) + V[0];
		break;

	case 0xC000: // CXNN: Sets VX to a random number and NN
		V[x] = (rand() % 0xFF) & z;
		pc += 2;
		break;

	case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++) {
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++) {
				if ((pixel & (0x80 >> xline)) != 0) {
					if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
						V[0xF] = 1;
					}
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		drawFlag = true;
		pc += 2;
	}
	break;

	case 0xE000:
		switch (opcode & 0x00FF) {
			case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
				if (key[V[x]] != 0) {
					pc += 4;
				} else {
					pc += 2;
				}
				break;

			case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
				if (key[V[x]] == 0){
					pc += 4;
				} else {
					pc += 2;
				}
				break;
			}
		break;

	case 0xF000:
		switch (opcode & 0x00FF) {
			case 0x0007: // FX07: Sets VX to the value of the delay timer
				V[x] = delay_timer;
				pc += 2;
				break;

			case 0x000A: // FX0A: A key press is awaited, and then stored in VX		
			{
				bool keyPress = false;
				for (int i = 0; i < 16; ++i)
				{
					if (key[i] != 0)
					{
						V[x] = i;
						keyPress = true;
					}
				}
				if (!keyPress) {
					return;
				}
				pc += 2;
			}
			break;

			case 0x0015: // FX15: Sets the delay timer to VX
				delay_timer = V[x];
				pc += 2;
				break;

			case 0x0018: // FX18: Sets the sound timer to VX
				sound_timer = V[x];
				pc += 2;
				break;

			case 0x001E: // FX1E: Adds VX to I
				if (I + V[x] > 0xFFF) {
					V[0xF] = 1;
				} else {
					V[0xF] = 0;
				}
				I += V[x];
				pc += 2;
				break;

			case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
				I = V[x] * 0x5;
				pc += 2;
				break;

			case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
				memory[I] = V[x] / 100;
				memory[I + 1] = (V[x] / 10) % 10;
				memory[I + 2] = (V[x] % 100) % 10;
				pc += 2;
				break;

			case 0x0055: // FX55: Stores V0 to VX in memory starting at address I
				for (int i = 0; i <= x; ++i) {
					memory[I + i] = V[i];
				}
				I += x + 1;
				pc += 2;
				break;

			case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I
				for (int i = 0; i <= x; ++i) {
					V[i] = memory[I + i];
				}
				I += x + 1;
				pc += 2;
				break;

		}
		break;

	}
	// Update delay timer
	if (delay_timer > 0) {
		--delay_timer;
	}

	// Update sound timer
	if (sound_timer > 0) {
		if (sound_timer == 1) {
			Beep(500, 500);
		}
		--sound_timer;
	}
}