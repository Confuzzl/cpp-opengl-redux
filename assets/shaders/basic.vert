#version 450 core

layout (location = 0) in vec3 pos;

uniform ProjectionBlock {
	mat4 proj;
};
uniform CameraBlock {
	mat4 cam;
};
uniform mat4 model;

out vec3 world_pos;

void main() {
	gl_Position = proj * cam * model * vec4(pos, 1.0);
	world_pos = (model * vec4(pos, 1.0)).xyz;
}