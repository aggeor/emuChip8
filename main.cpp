

#include "Chip8.h"
using namespace std;

Chip8 chip;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_bool done = SDL_FALSE;
SDL_Event event;
SDL_Rect r;
SDL_Texture* sdlTexture;
OPENFILENAME ofn;

char filename[MAX_PATH];

void setupGraphics() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize the SDL2 library: "<< SDL_GetError()<<std::endl;
    }
    window = SDL_CreateWindow("emuChip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 640, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
    }
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);

    if (!window_surface)
    {
        std::cout << "Failed to get the surface from the window: " << SDL_GetError() << std::endl;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cout << "Failed to initialize renderer: " << SDL_GetError() << std::endl;
    }
    sdlTexture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 32);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(1000);
}

void drawGraphics(unsigned char data[]) {
    SDL_RenderClear(renderer);
    for (int j = 0; j < 32; j++) {
        for (int i = 0; i < 64; i++) {            
            r.x = i*20;
            r.y = j*20;
            r.w = 20;
            r.h = 20;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            if (data[i + (j*64)]==1) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
            }
            SDL_RenderFillRect(renderer, &r);

        }
    }
    SDL_RenderPresent(renderer);
}

void tick() {
    // Emulate one cycle
    chip.emulateCycle();
    // If the draw flag is set, update the screen
    if (chip.drawFlag) {
        chip.drawFlag = false;
        drawGraphics(chip.gfx);
    }
    // Store key press state (Press and Release)
    chip.setKeys();
}

void selectRom() {

    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "All files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Select ROM file";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        std::cout << "Selected ROM file: \"" << filename << "\"\n";
    }
    else {
        switch (CommDlgExtendedError())
        {
        case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
        case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
        case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
        case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
        case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
        case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
        case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
        case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
        case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
        case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
        case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
        case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
        case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
        case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
        case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
        default: std::cout << "You cancelled.\n";
        }
    }
}


int main(int argc, char** argv) {
    selectRom();
    // Set up render system and register input callbacks
    setupGraphics();
    // Initialize the Chip8 system and load the game into the memory  
    chip.initialize();
    chip.debug = true;
    chip.loadGame(filename);
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    // Emulation loop
    for (;;)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float delay = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (delay > 5)
        {
            lastCycleTime = currentTime;
            tick();
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


