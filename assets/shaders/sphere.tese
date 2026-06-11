#version 460 core

//layout (quads, equal_spacing, ccw, point_mode) in;
layout (quads, equal_spacing, ccw) in;

uniform ProjectionBlock {
	mat4 proj;
};
uniform CameraBlock {
	mat4 cam;
};
uniform mat4 model;

in float tess_radius[];

out vec3 normal;
out vec3 world_pos;

const float PI = 3.141592;

void main()
{
    const vec3 center = gl_in[0].gl_Position.xyz;
    const float radius = tess_radius[0];

    const float u = gl_TessCoord.x, v = gl_TessCoord.y;
    const float theta = u * PI, phi = v * 2 * PI;

    const vec3 s = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    const vec4 pos = vec4(center + radius * s, 1.0);

    gl_Position = proj * cam * model * pos;
    world_pos = (model * pos).xyz;
    const mat3 normal_mat = mat3(transpose(inverse(model)));
    normal = normalize(normal_mat * s);
}