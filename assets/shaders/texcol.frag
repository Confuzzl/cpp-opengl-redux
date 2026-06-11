#version 460 core

in vec2 uv_out;

out vec4 color;

uniform sampler2D sampler;
uniform uint frag_color;

vec4 rgba() {
	const uint r = frag_color >> 24u & 0xffu;
	const uint g = frag_color >> 16u & 0xffu;
	const uint b = frag_color >> 8u & 0xffu;
	const uint a = frag_color & 0xffu;
	return vec4(r, g, b, a) / 255.0;
}

void main() {
	const vec4 col = texture(sampler, uv_out) * rgba();
	if (col.a == 0)
		discard;
	color = col;
}