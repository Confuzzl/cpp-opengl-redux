#version 460 core

uniform vec3 camera_pos;

layout (std140) uniform PhongData {
    vec4 light_pos_shininess; // w = shininess
    uint light_color;

    uint ambient_color; 
    float ambient_strength;
    
    uint diffuse_color;
    float diffuse_strength;
    
    uint specular_color;
    float specular_strength;
};

uniform sampler2D sampler;

in vec3 world_pos;
in vec2 uv;
in vec3 normal;

out vec4 color;

vec3 rgb(const uint color) {
	const uint r = color >> 24u & 0xffu;
	const uint g = color >> 16u & 0xffu;
	const uint b = color >> 8u & 0xffu;
	return vec3(r, g, b) / 255.0;
}

vec3 reflector_combine(const uint color, const float strength) {
    return rgb(color) * strength;
}

void main() {
    const vec3 light_pos = light_pos_shininess.xyz;
    const float shininess = light_pos_shininess.w;

    const vec3 light_dir = normalize(light_pos - world_pos); // light as a point light
    const vec3 view = normalize(camera_pos - world_pos);
    const vec3 ref = reflect(-light_dir, normal);

    const float diffuse_factor = max(0, dot(normal, light_dir));
    const float specular_factor = pow(max(0, dot(view, ref)), shininess);

    const vec3 ambient = reflector_combine(ambient_color, ambient_strength);
    const vec3 diffuse = reflector_combine(diffuse_color, diffuse_strength) * diffuse_factor;
    const vec3 specular = reflector_combine(specular_color, specular_strength) * specular_factor;

    const vec4 tex = texture(sampler, uv);

    color = vec4(ambient + diffuse + specular, 1.0) * tex;
}
