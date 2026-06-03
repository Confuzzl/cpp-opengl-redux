#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 uv_in;

uniform ProjectionBlock {
	mat4 proj;
};
uniform CameraBlock {
	mat4 cam;
};
uniform mat4 model;

out vec3 world_pos;
out mat3 tbn;
out vec2 uv;

// https://learnopengl.com/Advanced-Lighting/Normal-Mapping
void main() {
	gl_Position = proj * cam * model * vec4(pos, 1.0);
	uv = uv_in;
	world_pos = (model * vec4(pos, 1.0)).xyz;

	const vec3 n = normalize(vec3(model * vec4(norm, 0.0)));
	const vec3 t = normalize(vec3(model * vec4(tangent, 0.0)));
	const vec3 b = normalize(cross(n, t));
	tbn = mat3(t, b, n);
}
