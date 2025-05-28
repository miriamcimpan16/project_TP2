#pragma once

#include <SDL2/SDL.h>

#include "../types.h"
#include "render.h"

SDL_Window *render_init_window(u32 width, u32 height);
void render_init_quad(u32 *vao, u32 *vbo, u32 *ebo);
void render_init_color_texture(u32 *texture);
void render_init_shaders(u32 *shader_default,u32 *shader_batch,f32 render_width, f32 render_height);
void render_init_batch_quads(u32 *vao, u32 *vbo, u32 *ebo);
void render_init_line(u32 *vao, u32 *vbo);
u32 render_shader_create(const char *path_vert, const char *path_frag);
bool render_init_freetype(const char *font_path);
void render_init_text_shader(u32 *shader_text, f32 width, f32 height);
void render_init_text_vao(u32 *vao, u32 *vbo);
void render_draw_char(u32 shader_text, u32 vao_text, u32 vbo_text,char c, float x, float y, float scale, vec3 color);
void render_draw_test_quad(float x, float y, float width, float height);







