/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <SDL/texture_sheet_loader.hpp>
#include <physics/mat3.hpp>

namespace Animations
{

void init_sprite_voxelizer();
void teardown_sprite_voxelizer();
void load_sprite_voxelizer();

void draw_voxelized_sprite(int sprite_id, const struct Mat4& rotation_matrix);

bool draw_voxelized_sprite_gl_begin(GLint cull_mode);
void draw_voxelized_sprite_gl_end();

void render_voxelized_sprite_fbo();

void use_voxelized_sprite_fbo();
void unuse_voxelized_sprite_fbo();
void render_voxelized_sprite_fbo();

}   // Animations
