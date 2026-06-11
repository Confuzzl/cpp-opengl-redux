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
in float tess_height[];

out vec3 normal;
out vec3 world_pos;

patch in float rings;

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

    const float u = gl_TessCoord.x, v = gl_TessCoord.y;

    const mat3 normal_mat = mat3(transpose(inverse(model)));
    vec3 n;

    vec3 s;
        /*
        +----------+ 1
        | SIDE     |
        +----------+
        | ...      |
        +----------+ 2 / r
        | BOTTOM 2 | // needed to make bottom normal flat
        +----------+ 1 / r
        | BOTTOM 1 |
        +----------+ 0
        0          1
        */
    const float ring_step = 1.0 / rings;
    if (v == 0) {
        s = vec3(0, 0, 0);
        n = vec3(0, -1, 0);
    } else {
        const float theta = u * 2 * PI;
        if (compare_float(v, ring_step)) {
            s = vec3(sin(theta), 0, cos(theta));
            n = vec3(0, -1, 0);
        } else {
            const float side_start = ring_step; // [2/r, 1] -> [0, 1 - 2/r = (r - 2)/r] -> [0, r - 2] -> [0, 1]

            const float t = (v - 2 * ring_step) * rings / (rings - 2);
            const float r = radius * (1 - t);
            const float h = height * t;

            const vec3 base = vec3(sin(theta), 0, cos(theta));
            const vec3 tip = vec3(0, height, 0);
            const vec3 to_tip = tip - base;
            const vec3 to_right = cross(to_tip, base);
            n = normalize(cross(to_right, to_tip));

            s = r * base;
            s.y = h;
        }
    }
    normal = normalize(normal_mat * n);

    const vec4 pos = vec4(center + s, 1.0);

    gl_Position = proj * cam * model * pos;
    world_pos = (model * pos).xyz;
}