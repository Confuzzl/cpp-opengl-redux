#version 460 core

const float RESOLUTION = 16;
const float RINGS = 16; // increase for better smooth shading near tip


layout (vertices = 1) out;

in float vert_radius[];
in float vert_height[];

out float tess_radius[];
out float tess_height[];

patch out float rings;

void main() {
	rings = RINGS;

	gl_TessLevelOuter[0] = RINGS;
	gl_TessLevelOuter[1] = RESOLUTION;
	gl_TessLevelOuter[2] = RINGS;
	gl_TessLevelOuter[3] = RESOLUTION;
	
	gl_TessLevelInner[0] = RESOLUTION;
	gl_TessLevelInner[1] = RINGS;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tess_radius[gl_InvocationID] = vert_radius[gl_InvocationID];
	tess_height[gl_InvocationID] = vert_height[gl_InvocationID];
}