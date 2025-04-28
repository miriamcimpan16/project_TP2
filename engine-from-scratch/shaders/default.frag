#version 330 core
out vec4 frag_color;

in vec2 uvs;

uniform vec4 color;
uniform sampler2D texture_id;

void main() {
    float alpha = texture(texture_id, uvs).r; // fonturile sunt în GL_RED, deci folosim `.r`
    frag_color = vec4(color.rgb, color.a * alpha); // text cu alpha corect
}



