gcc -Wall -std=c99 platform/sdl/platform.c src/*.c -I/opt/homebrew/include -I/opt/homebrew/include/ -L/opt/homebrew/lib -lSDL2 -D_THREAD_SAFE -o build/sdl/sdl_renderer
