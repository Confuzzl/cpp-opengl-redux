#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in int test1;
layout (location = 2) in int test2;

uniform ProjectionBlock {
	mat4 proj;
};
uniform CameraBlock {
	mat4 cam;
};
uniform mat4 model;

out vec3 world_pos;

void main() {
	vec3 p = pos;
	p.y = float(test1 + test2);
	gl_Position = proj * cam * model * vec4(p, 1.0);
	world_pos = (model * vec4(pos, 1.0)).xyz;
}
