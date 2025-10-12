#ifndef TEXTURE
#define TEXTURE

#include <stdlib.h>

extern const uint8_t REDBRICK_TEXTURE[];

typedef struct {
	float u;
	float v;
} tex2d;

typedef struct {
	int width;
	int height;
	uint32_t *data;
} Texture;

void load_texture(unsigned char *filename, Texture *t);
void set_redbrick_texture(Texture *t);

#endif
