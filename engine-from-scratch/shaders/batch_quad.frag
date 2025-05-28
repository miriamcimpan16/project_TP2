#version 330 core
out vec4 o_color;

in vec4 v_color;
in vec2 v_uvs;
flat in int v_texture_slot;

uniform sampler2D texture_slot_0;
uniform sampler2D texture_slot_1;
uniform sampler2D texture_slot_2;
uniform sampler2D texture_slot_3;
uniform sampler2D texture_slot_4;
uniform sampler2D texture_slot_5;
uniform sampler2D texture_slot_6;
uniform sampler2D texture_slot_7;

vec4 sample_texture(int slot, vec2 uv) {
    vec4 tex_color = vec4(1.0);
    if (slot == 0) tex_color = texture(texture_slot_0, uv);
    else if (slot == 1) tex_color = texture(texture_slot_1, uv);
    else if (slot == 2) tex_color = texture(texture_slot_2, uv);
    else if (slot == 3) tex_color = texture(texture_slot_3, uv);
    else if (slot == 4) tex_color = texture(texture_slot_4, uv);
    else if (slot == 5) tex_color = texture(texture_slot_5, uv);
    else if (slot == 6) tex_color = texture(texture_slot_6, uv);
    else if (slot == 7) tex_color = texture(texture_slot_7, uv);

    // Dacă textura are doar canalul R (text/font), tratăm ca alpha
    if (tex_color.g == 0.0 && tex_color.b == 0.0) {
        return vec4(v_color.rgb, v_color.a * tex_color.r);
    }

    return tex_color * v_color;
}

void main() {
    o_color = sample_texture(v_texture_slot, v_uvs);
}

