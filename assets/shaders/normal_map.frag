#version 460 core

//uniform uint frag_color;
uniform vec3 light;
uniform uint light_color;
uniform vec3 camera_pos;

in vec3 world_pos;
in mat3 tbn;
//in vec3 normal;
in vec2 uv;

out vec4 color;

const float ambient_strength = 0.1;
const float specular_strength = 1.0;

vec3 rgb(const uint i) {
	const uint r = i >> 24u & 0xffu;
	const uint g = i >> 16u & 0xffu;
	const uint b = i >> 8u & 0xffu;
	return vec3(r, g, b) / 255.0;
}

layout (binding = 0) uniform sampler2D diffuse_map;
layout (binding = 1) uniform sampler2D normal_map;

// https://learnopengl.com/Advanced-Lighting/Normal-Mapping
void main() {
	const vec3 sampled_normal = texture(normal_map, uv).rgb;
	const vec3 normalized_normal = sampled_normal * 2.0 - 1.0;
	const vec3 n = normalize(tbn * normalized_normal);

	const vec3 light_dir = normalize(light - world_pos); // light as a point light
	const float diffuse = max(0, dot(n, light_dir));
	const vec3 view = normalize(camera_pos - world_pos);
	const vec3 ref = reflect(-light_dir, n);
	const float spec = specular_strength * pow(max(0, dot(view, ref)), 512);
	color = vec4((ambient_strength + diffuse + spec) * texture(diffuse_map, uv).rgb, 1.0);
}
