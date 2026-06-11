#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv_in;

uniform ProjectionBlock {
	mat4 proj;
};
uniform CameraBlock {
	mat4 cam;
};
uniform mat4 model;

out vec2 uv_out;

void main() {
	gl_Position = proj * cam * model * vec4(pos, 1.0);
	uv_out = uv_in;
}
