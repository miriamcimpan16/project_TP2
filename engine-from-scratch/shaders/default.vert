#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uvs;
layout (location = 2) in vec4 a_color;
layout (location = 3) in float a_texture_slot;

out vec2 uvs;
out vec4 vert_color;
out float texture_slot;

uniform mat4 projection;
uniform mat4 model;

void main() {
    uvs = a_uvs;
    vert_color = a_color;
    texture_slot = a_texture_slot;
    gl_Position = projection * model * vec4(a_pos, 1.0);
}







