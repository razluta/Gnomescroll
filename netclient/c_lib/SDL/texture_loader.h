#pragma once

#include <compat_gl.h>

struct Texture {
    int tex;
    int w;
    int h;
};

int init_image_loader();

SDL_Surface* _load_image(char *file);
struct Texture _load_image_create_texture(char *file);

int create_texture_from_file(char* filename, int* tex);
int create_texture_from_surface(SDL_Surface* surface, int* tex);
