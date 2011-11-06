#ifndef texture_loader_h
#define texture_loader_h

#include <compat_gl.h>

typedef struct {
    int tex;
    int w;
    int h;
} Texture;

int _init_image_loader();
SDL_Surface* _load_image(char *file);
int _create_hud_texture(char *file);

int _create_block_texture(char *file);
int _create_texture(SDL_Surface* surface);

int _create_hud_texture2(SDL_Surface* surface);
void _draw_loaded_hud_texture(int x, int y);
void _load_hud_texture(char *file);

int _blit_sprite2(int tex, float x0, float y0, float x1, float y1, float z);

/*
void _draw_block_selector(int x, int y);
void _load_block_selector_texture(char *file, int scale);
*/

#endif
