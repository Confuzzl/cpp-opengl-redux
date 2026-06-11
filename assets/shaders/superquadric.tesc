#version 460 core

const float RESOLUTION = 32;

layout (vertices = 1) out;

in vec3 vert_scales[];
in vec3 vert_powers[];

out vec3 tess_scales[];
out vec3 tess_powers[];

void main() {
	gl_TessLevelOuter[0] = RESOLUTION;
	gl_TessLevelOuter[1] = 2 * RESOLUTION;
	gl_TessLevelOuter[2] = RESOLUTION;
	gl_TessLevelOuter[3] = 2 * RESOLUTION;
	
	gl_TessLevelInner[0] = 2 * RESOLUTION;
	gl_TessLevelInner[1] = RESOLUTION;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tess_scales[gl_InvocationID] = vert_scales[gl_InvocationID];
	tess_powers[gl_InvocationID] = vert_powers[gl_InvocationID];
}