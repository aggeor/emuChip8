
#include <iostream>
#include <fstream>

#include <SDL.h>
#include <chrono>
#include <iostream>
#include <windows.h>
using namespace std;

class Chip8
{
	public:
		unsigned short opcode;
		unsigned char memory[4096];
		unsigned char V[16];
		unsigned short I;
		unsigned short pc;
		unsigned char gfx[64*32];
		unsigned char delay_timer;
		unsigned char sound_timer;
		unsigned short stack[16];
		unsigned short sp;
		unsigned char key[16];
		bool drawFlag;
		bool debug;

		void initialize();

		void loadGame(const char* filename);

		void emulateCycle();
		void setKeys();
		void disassemble(unsigned short opcode, int x, int y, int z);
		
		
};

