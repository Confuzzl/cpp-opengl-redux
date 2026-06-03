#version 450 core

uniform uint frag_color;
uniform vec3 light;
uniform uint light_color;

in vec3 normal;

out vec4 color;

vec3 rgb(const uint i) {
	const uint r = i >> 24u & 0xffu;
	const uint g = i >> 16u & 0xffu;
	const uint b = i >> 8u & 0xffu;
	return vec3(r, g, b) / 255.0;
}

void main() {
	const float d = max(0, dot(normal, normalize(light))); // light as a directional light
	color = vec4(d * rgb(light_color) * rgb(frag_color), 1.0);
}
