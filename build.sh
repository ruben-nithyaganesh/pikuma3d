mkdir -p build
gcc -Wall -std=c99 src/*.c -I/opt/homebrew/include -I/opt/homebrew/include/ -L/opt/homebrew/lib -lSDL2 -D_THREAD_SAFE -o build/game