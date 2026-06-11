#version 460 core

layout (location = 0) in vec3 center;
layout (location = 1) in float inner_radius;
layout (location = 2) in float outer_radius;

out float vert_inner_radius;
out float vert_outer_radius;

void main() {
	gl_Position = vec4(center, 0.0);
	vert_inner_radius = inner_radius;
	vert_outer_radius = outer_radius;
}
