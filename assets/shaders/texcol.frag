#version 450 core

in vec2 uv_out;

out vec4 color;

uniform sampler2D sampler;
uniform uint frag_color;

vec4 rgba() {
	const uint r = frag_color >> 24 & 0xff;
	const uint g = frag_color >> 16 & 0xff;
	const uint b = frag_color >> 8 & 0xff;
	const uint a = frag_color & 0xff;
	return vec4(r, g, b, a) / 255.0;
}

void main() {
	const vec4 col = texture(sampler, uv_out) * rgba();
	if (col.a == 0)
		discard;
	color = col;
}