#version 450 core

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
in float tess_height[];

out vec3 normal;
out vec3 world_pos;

const float PI = 3.141592;

const float EPS = 0.001;
bool compare_float(const float a, const float b) {
    return abs(a - b) < EPS;
}

void main()
{
    const vec3 center = gl_in[0].gl_Position.xyz;
    const float radius = tess_radius[0];
    const float height = tess_height[0];
    const float half_height = height / 2;

    const float u = gl_TessCoord.x, v = gl_TessCoord.y;

    const mat3 normal_mat = mat3(transpose(inverse(model)));
    vec3 n;

    vec3 s;
    /*
    +----------+ 1
    | TOP 1    |
    +----------+ 0.8
    | TOP 2    | // needed to make top normal flat
    +----------+ 0.6
    | SIDES    |
    +----------+ 0.4
    | BOTTOM 2 | // needed to make bottom normal flat
    +----------+ 0.2
    | BOTTOM 1 |
    +----------+ 0
    0         1
    */
    if (v == 0) {
        s = vec3(0, -half_height, 0);
        n = vec3(0, -1, 0);
    } else if (v == 1) {
        s = vec3(0, +half_height, 0);
        n = vec3(0, +1, 0);
    } else {
        const float theta = u * 2 * PI;
        const vec2 o = vec2(radius * sin(theta), radius * cos(theta));
        if (compare_float(v, 0.2)) {
            s = vec3(o.x, -half_height, o.y);
            n = vec3(0, -1, 0);
        } else if (compare_float(v, 0.8)) {
            s = vec3(o.x, +half_height, o.y);
            n = vec3(0, +1, 0);
        } else {
            // [0.4, 0.6] -> [-0.1, +0.1] -> [-0.5, +0.5]
            const float h = (v - 0.5) * 5 * height;
            s = vec3(o.x, h, o.y);
            n = vec3(o.x, 0, o.y);
        }
    }
    normal = normalize(normal_mat * n);

    const vec4 pos = vec4(center + s, 1.0);

    gl_Position = proj * cam * model * pos;
    world_pos = (model * pos).xyz;
}