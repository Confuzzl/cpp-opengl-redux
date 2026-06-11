#version 460 core

uniform uint frag_color;
uniform uint light_color;
uniform vec3 light;

in vec3 geom_normal;

out vec4 color;

vec3 rgb(const uint i) {
	const uint r = i >> 24u & 0xffu;
	const uint g = i >> 16u & 0xffu;
	const uint b = i >> 8u & 0xffu;
	return vec3(r, g, b) / 255.0;
}

void main() {
	const float d = max(0, dot(geom_normal, normalize(light))); // light as a directional light
	color = vec4(d * rgb(light_color) * rgb(frag_color), 1.0);
}
