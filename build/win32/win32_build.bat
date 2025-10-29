del *.exe
del *.obj
del *.ilk
del *.pdb

cl -Zi /Fo: build\win32\\ /Fd: build\win32\\ /Fe: build\win32\win32_renderer.exe platform/win32/platform.c src/*.c user32.lib Gdi32.lib Kernel32.lib
