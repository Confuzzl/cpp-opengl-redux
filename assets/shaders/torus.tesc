#version 460 core

layout (vertices = 1) out;

uniform float resolution = 16;

in float vert_inner_radius[];
in float vert_outer_radius[];

out float tess_inner_radius[];
out float tess_outer_radius[];

void main() {
	gl_TessLevelOuter[0] = resolution;
	gl_TessLevelOuter[1] = resolution;
	gl_TessLevelOuter[2] = resolution;
	gl_TessLevelOuter[3] = resolution;
	
	gl_TessLevelInner[0] = resolution;
	gl_TessLevelInner[1] = resolution;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tess_inner_radius[gl_InvocationID] = vert_inner_radius[gl_InvocationID];
	tess_outer_radius[gl_InvocationID] = vert_outer_radius[gl_InvocationID];
}