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

in vec3 tess_scales[];
in vec3 tess_powers[];

out vec3 normal;
out vec3 world_pos;

const float PI = 3.141592;

const float EPS = 0.001;
bool compare_float(const float a, const float b) {
    return abs(a - b) < EPS;
}

float my_pow(const float a, const float x) {
//    if (compare_float(a, 0) && compare_float(x, 0)) return 1;
    if (compare_float(a, 0)) return 0;
    return pow(a, x);
}
float f(const float a, const float b) {
    const float x = sin(a);
    const float s = sign(x);
    if (compare_float(s, 0)) return 0;
    return s * my_pow(abs(x), b);
}
float g(const float a, const float b) {
    const float x = cos(a);
    const float s = sign(x);
    if (compare_float(s, 0)) return 0;
    return s * my_pow(abs(x), b);
}
float zero_if_zero(const float x) {
    return compare_float(x, 0) ? 0 : 1;
}

void main()
{
    const vec3 center = gl_in[0].gl_Position.xyz;
    const vec3 scales = tess_scales[0], powers = tess_powers[0];
    const float A = scales.x, B = scales.y, C = scales.z;
    const float r = powers.x, s = powers.y, t = powers.z;

    const float u = (gl_TessCoord.x - 0.5) * 2 * PI;
    const float v = (gl_TessCoord.y - 0.5) * PI;

    // https://en.wikipedia.org/wiki/Superquadrics#Parametric_description
    const float rho = 2 / r, sig = 2 / s, tau = 2 / t;
    const vec3 p = vec3(
        A * g(v, rho) * g(u, rho),
        B * g(v, sig) * f(u, sig),
        C * f(v, tau)
    );
    const vec4 pos = vec4(center + p, 1.0);


    gl_Position = proj * cam * model * pos;
    world_pos = (model * pos).xyz;

    // check etc/superquadric.md
    const float cosu = cos(u), sinu = sin(u);
    const float cosv = cos(v), sinv = sin(v);
    const vec3 d_u = vec3(
        A * g(v, rho) * -rho * sinu * my_pow(abs(cosu), rho - 1),
        B * g(v, sig) * +sig * cosu * my_pow(abs(sinu), sig - 1),
        0
    );
    const vec3 d_v = vec3(
        A * g(u, rho) * -rho * sinv * my_pow(abs(cosv), rho - 1),
        B * f(u, sig) * -sig * sinv * my_pow(abs(cosv), sig - 1),
        C * tau * cos(v) * my_pow(abs(sinv), tau - 1)
    );
    vec3 n = cross(d_u, d_v);
    if (compare_float(dot(d_u, d_u), 0) || compare_float(dot(d_v, d_v), 0)) {
        n = p;
    }
    const mat3 normal_mat = mat3(transpose(inverse(model)));
    normal = normalize(normal_mat * n);
}