#include <glad/glad.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../global.h"
#include "render.h"
#include "../array_list.h"
#include "../util.h"
#include "render_internal.h"
#include <ft2build.h>
#include FT_FREETYPE_H
FT_Library ft;
FT_Face face;

static f32 window_width = 1920;
static f32 window_height = 1080;
static f32 render_width = 640;
static f32 render_height = 360;
static f32 scale = 3;

static u32 vao_quad;
static u32 vbo_quad;
static u32 ebo_quad;
static u32 vao_line;
static u32 vbo_line;
static u32 shader_default;
static u32 texture_color;
static u32 vao_batch;
static u32 vbo_batch;
static u32 ebo_batch;
static u32 shader_batch;
static Array_List *list_batch;

typedef struct {
    GLuint texture_id;
    vec2 size;
    vec2 bearing;
    u32 advance;
} Character;

Character characters[128]; // ASCII 0 - 127


void render_text_sprite(Sprite_Sheet font_sprite_sheet,const char* text, float x, float y, vec4 color, u32 texture_slots[8]) {
    const char* p;
    float start_x = x;
    for (p = text; *p; p++) {
        if (*p == ' ') {
            x += font_sprite_sheet.cell_width; // Spațiu
            continue;
        }

        u8 c = (u8)*p;
        u32 row = (c - 32) / 16; // 16 coloane per rând
        u32 column = (c - 32) % 16;

        vec2 position = { x, y };
        render_sprite_sheet_frame(&font_sprite_sheet, row, column, position, false, color, texture_slots);

        x += font_sprite_sheet.cell_width; // Avansează la dreapta
    }
}

void render_init_freetype() {
    if (FT_Init_FreeType(&ft)) {
        printf("Eroare la initializarea FreeType!\n");
        return;
    }

    if (FT_New_Face(ft, "C:\\Users\\Deborah\\OneDrive\\Desktop\\project\\engine-from-scratch\\shaders\\font.ttf", 0, &face)) {
        printf("Eroare la incarcarea fontului!\n");
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Pentru texturi 1 byte/px

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("Eroare la incarcarea caracterului '%c'\n", c);
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
		if (texture == 0) {
			printf("Eroare la generarea texturii pentru caracterul '%c'\n", c);
		}
		
        glBindTexture(GL_TEXTURE_2D, texture);
		if (face->glyph->bitmap.buffer == NULL) {
			printf("Eroare la încărcarea buffer-ului pentru caracterul '%c'\n", c);
		}
		
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
GLenum err = glGetError();
if (err != GL_NO_ERROR) {
    printf("Eroare OpenGL după glTexParameteri(): %d\n", err);
}

        characters[c].texture_id = texture;
        characters[c].size[0] = face->glyph->bitmap.width;
        characters[c].size[1] = face->glyph->bitmap.rows;
        characters[c].bearing[0] = face->glyph->bitmap_left;
        characters[c].bearing[1] = face->glyph->bitmap_top;
        characters[c].advance = face->glyph->advance.x;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void render_text(const char* text, float x, float y, vec4 color) {
	glUseProgram(shader_default);
    glUniform1i(glGetUniformLocation(shader_default, "text"), 0);


    const char* p;
    for (p = text; *p; p++) {
        Character ch = characters[(unsigned char)*p];

        float xpos = x + ch.bearing[0];
        float ypos = y - (ch.size[1] - ch.bearing[1]);

        vec2 size = { ch.size[0], ch.size[1] };
        vec2 bottom_left = { xpos, ypos };

        glBindTexture(GL_TEXTURE_2D, ch.texture_id);
        append_quad(bottom_left, size, (void*)(intptr_t)ch.texture_id, color, 0);

        x += (ch.advance >> 6); // de la 1/64 px la px
    }
	printf("[DEBUG] Called render_text: x = %f, y = %f\n", x, y);
}

void render_game_over(Sprite_Sheet font_sprite_sheet,u32 texture_slots[8]) {

    render_text_sprite(font_sprite_sheet,"GAME OVER", 100, 200, (vec4){1.0, 0.0, 0.0, 1.0}, texture_slots);
}


void render_cleanup_freetype() {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


SDL_Window *render_init(void) {
	SDL_Window *window = render_init_window(window_width, window_height);

	render_init_quad(&vao_quad, &vbo_quad, &ebo_quad);
	render_init_batch_quads(&vao_batch, &vbo_batch, &ebo_batch);
	render_init_line(&vao_line, &vbo_line);
	render_init_shaders(&shader_default, &shader_batch, render_width, render_height);
	render_init_color_texture(&texture_color);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	list_batch = array_list_create(sizeof(Batch_Vertex), 8);

	stbi_set_flip_vertically_on_load(1);

	return window;
}

static i32 find_texture_slot(u32 texture_slots[8], u32 texture_id) {
    for (i32 i = 1; i < 8; ++i) {
        if (texture_slots[i] == texture_id) {
            return i;
        }
    }

    return -1;
}

static i32 try_insert_texture(u32 texture_slots[8], u32 texture_id) {
    i32 index = find_texture_slot(texture_slots, texture_id);
    if (index > 0) {
        return index;
    }

    for (i32 i = 1; i < 8; ++i) {
        if (texture_slots[i] == 0) {
            texture_slots[i] = texture_id;
            return i;
        }
    }

    return -1;
}

void render_begin(void) {
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	list_batch->len = 0;
}

static void render_batch(Batch_Vertex *vertices, usize count, u32 texture_ids[8]) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo_batch);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(Batch_Vertex), vertices);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_color);

    for (u32 i = 1; i < 8; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
    }

	glUseProgram(shader_batch);
	glBindVertexArray(vao_batch);

	glDrawElements(GL_TRIANGLES, (count >> 2) * 6, GL_UNSIGNED_INT, NULL);
}

static void append_quad(vec2 position, vec2 size, vec4 texture_coordinates, vec4 color, f32 texture_slot) {
	vec4 uvs = {0, 0, 1, 1};

	if (texture_coordinates != NULL) {
		memcpy(uvs, texture_coordinates, sizeof(vec4));
	}

	array_list_append(list_batch, &(Batch_Vertex){
		.position = {position[0], position[1]},
		.uvs = {uvs[0], uvs[1]},
		.color = {color[0], color[1], color[2], color[3]},
        .texture_slot = texture_slot,
	});

	array_list_append(list_batch, &(Batch_Vertex){
		.position = {position[0] + size[0], position[1]},
		.uvs = {uvs[2], uvs[1]},
		.color = {color[0], color[1], color[2], color[3]},
        .texture_slot = texture_slot,
	});

	array_list_append(list_batch, &(Batch_Vertex){
		.position = {position[0] + size[0], position[1] + size[1]},
		.uvs = {uvs[2], uvs[3]},
		.color = {color[0], color[1], color[2], color[3]},
        .texture_slot = texture_slot,
	});

	array_list_append(list_batch, &(Batch_Vertex){
		.position = {position[0], position[1] + size[1]},
		.uvs = {uvs[0], uvs[3]},
		.color = {color[0], color[1], color[2], color[3]},
        .texture_slot = texture_slot,
	});
}

void render_end(SDL_Window *window, u32 batch_texture_ids[8]) {
	render_batch(list_batch->items, list_batch->len, batch_texture_ids);

	SDL_GL_SwapWindow(window);
}

void render_quad(vec2 pos, vec2 size, vec4 color) {
	glUseProgram(shader_default);

	mat4x4 model;
	mat4x4_identity(model);

	mat4x4_translate(model, pos[0], pos[1], 0);
	mat4x4_scale_aniso(model, model, size[0], size[1], 1);

	glUniformMatrix4fv(glGetUniformLocation(shader_default, "model"), 1, GL_FALSE, &model[0][0]);
	glUniform4fv(glad_glGetUniformLocation(shader_default, "color"), 1, color);

	glBindVertexArray(vao_quad);

	glBindTexture(GL_TEXTURE_2D, texture_color);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
}

void render_line_segment(vec2 start, vec2 end, vec4 color) {
	glUseProgram(shader_default);
	glLineWidth(3);

	f32 x = end[0] - start[0];
	f32 y = end[1] - start[1];
	f32 line[6] = {0, 0, 0, x, y, 0};

	mat4x4 model;
	mat4x4_translate(model, start[0], start[1], 0);

	glUniformMatrix4fv(glGetUniformLocation(shader_default, "model"), 1, GL_FALSE, &model[0][0]);
	glUniform4fv(glGetUniformLocation(shader_default, "color"), 1, color);

	glBindTexture(GL_TEXTURE_2D, texture_color);
	glBindVertexArray(vao_line);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line), line);
	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);
}

void render_quad_line(vec2 pos, vec2 size, vec4 color) {
	vec2 points[4] = {
		{pos[0] - size[0] * 0.5, pos[1] - size[1] * 0.5},
		{pos[0] + size[0] * 0.5, pos[1] - size[1] * 0.5},
		{pos[0] + size[0] * 0.5, pos[1] + size[1] * 0.5},
		{pos[0] - size[0] * 0.5, pos[1] + size[1] * 0.5},
	};

	render_line_segment(points[0], points[1], color);
	render_line_segment(points[1], points[2], color);
	render_line_segment(points[2], points[3], color);
	render_line_segment(points[3], points[0], color);
}

void render_aabb(f32 *aabb, vec4 color) {
	vec2 size;
	vec2_scale(size, &aabb[2], 2);
	render_quad_line(&aabb[0], size, color);
}

f32 render_get_scale() {
	return scale;
}

void render_sprite_sheet_init(Sprite_Sheet *sprite_sheet, const char *path, f32 cell_width, f32 cell_height) {
	glGenTextures(1, &sprite_sheet->texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite_sheet->texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, channel_count;
	u8 *image_data = stbi_load(path, &width, &height, &channel_count, 0);
	if (!image_data) {
		ERROR_EXIT("Failed to load image: %s\n", path);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	sprite_sheet->width = (f32)width;
	sprite_sheet->height = (f32)height;
	sprite_sheet->cell_width = cell_width;
	sprite_sheet->cell_height = cell_height;
}

static void calculate_sprite_texture_coordinates(vec4 result, f32 row, f32 column, f32 texture_width, f32 texture_height, f32 cell_width, f32 cell_height) {
	f32 w = 1.0 / (texture_width / cell_width);
	f32 h = 1.0 / (texture_height / cell_height);
	f32 x = column * w;
	f32 y = row * h;
	result[0] = x;
	result[1] = y;
	result[2] = x + w;
	result[3] = y + h;
}

void render_sprite_sheet_frame(Sprite_Sheet *sprite_sheet, f32 row, f32 column, vec2 position, bool is_flipped, vec4 color, u32 texture_slots[8]) {
	vec4 uvs;
	calculate_sprite_texture_coordinates(uvs, row, column, sprite_sheet->width, sprite_sheet->height, sprite_sheet->cell_width, sprite_sheet->cell_height);

	if (is_flipped) {
		f32 tmp = uvs[0];
		uvs[0] = uvs[2];
		uvs[2] = tmp;
	}

	vec2 size = {sprite_sheet->cell_width, sprite_sheet->cell_height};
	vec2 bottom_left = {position[0] - size[0] * 0.5, position[1] - size[1] * 0.5};

    i32 texture_slot = try_insert_texture(texture_slots, sprite_sheet->texture_id);
    if (texture_slot == -1) {
        // TODO: Flush buffer
    }
	append_quad(bottom_left, size, uvs, color, (f32)texture_slot);
}







