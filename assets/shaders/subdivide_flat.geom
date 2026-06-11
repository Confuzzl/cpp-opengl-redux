#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3 * 4 * 4 * 4) out;

uniform ProjectionBlock {
	mat4 proj;
};
uniform mat4 cam;
uniform mat4 model;

const int MAX_LEVEL = 3;
uniform int level = 0;
//uniform int face_normal;

in vec3 normal[];
out vec3 geom_normal;

void main() {
	const int l = min(level, MAX_LEVEL);
	if (l == 0) {
		geom_normal = normal[0];
		for (int i = 0; i < 3; i++) {
			gl_Position = proj * cam * model * gl_in[0].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}