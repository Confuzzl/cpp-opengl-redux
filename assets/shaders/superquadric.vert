#version 450 core

layout (location = 0) in vec3 center;
layout (location = 1) in vec3 scales;
layout (location = 2) in vec3 powers;

out vec3 vert_scales;
out vec3 vert_powers;

void main() {
	gl_Position = vec4(center, 0.0);
	vert_scales = scales;
	vert_powers = powers;
}
