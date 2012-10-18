#pragma once

namespace HudContainer
{

extern GLuint SynthesizerTexture;
extern GLuint CraftingTexture;
extern GLuint StorageBlockTexture;
extern GLuint SmelterTexture;
extern GLuint CrusherTexture;

void init_texture();
void teardown_texture();

}   // HudContainer