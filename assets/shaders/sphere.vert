#version 460 core

layout (location = 0) in vec3 center;
layout (location = 1) in float radius;

out float vert_radius;

void main() {
	gl_Position = vec4(center, 0.0);
	vert_radius = radius;
}
