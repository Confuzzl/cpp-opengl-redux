#version 460 core

layout (location = 0) in vec3 base;
layout (location = 1) in float radius;
layout (location = 2) in float height;

out float vert_radius;
out float vert_height;

void main() {
	gl_Position = vec4(base, 0.0);
	vert_radius = radius;
	vert_height = height;
}
