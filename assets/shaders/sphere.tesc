#version 460 core

const float RESOLUTION = 16;

layout (vertices = 1) out;

in float vert_radius[];
out float tess_radius[];

void main() {
	gl_TessLevelOuter[0] = 2 * RESOLUTION; // multiply by 2 to account for y coordinate being mapped to 2*pi instead of pi
	gl_TessLevelOuter[1] = RESOLUTION;
	gl_TessLevelOuter[2] = 2 * RESOLUTION;
	gl_TessLevelOuter[3] = RESOLUTION;
	
	gl_TessLevelInner[0] = RESOLUTION;
	gl_TessLevelInner[1] = 2 * RESOLUTION;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tess_radius[gl_InvocationID] = vert_radius[gl_InvocationID];
}