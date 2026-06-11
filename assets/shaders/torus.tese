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

in float tess_inner_radius[];
in float tess_outer_radius[];

out vec3 normal;
out vec3 world_pos;

const float PI = 3.141592;

void main()
{
    const vec3 center = gl_in[0].gl_Position.xyz;
    const float r = tess_inner_radius[0];
    const float R = tess_outer_radius[0];

    const float u = gl_TessCoord.x, v = gl_TessCoord.y;
    const float theta = u * 2 * PI, phi = v * 2 * PI;

    // https://en.wikipedia.org/wiki/Torus#Geometry
    const vec3 s = vec3((R + r * sin(theta)) * cos(phi), -r * cos(theta), (R + r * sin(theta)) * sin(phi));
    const vec4 pos = vec4(center + s, 1.0);

    gl_Position = proj * cam * model * pos;
    world_pos = (model * pos).xyz;

    // https://en.wikipedia.org/wiki/Torus#Differential_geometry_for_the_ring_torus
    // can ignore the constant multiplicative terms since we normalize anyway
    const vec3 d_phi = vec3(-sin(phi), 0, cos(phi));
    const vec3 d_theta = vec3(cos(theta) * cos(phi), sin(theta), cos(theta) * sin(phi));

    const mat3 normal_mat = mat3(transpose(inverse(model)));
    normal = normalize(normal_mat * cross(d_theta, d_phi));
}