#version 430 core

out vec4 color;

uniform float VoxelSize;
uniform int Dimensions;
uniform sampler3D VoxelsTexture;

void main() {
	int voxelIndex = gl_VertexID;
	int xy = Dimensions * Dimensions;
	int x = voxelIndex % Dimensions;
	int y = (voxelIndex / Dimensions) % Dimensions;
	int z = voxelIndex / xy;
	vec3 pos = vec3(x, y, z);

	color = texture(VoxelsTexture, pos / float(Dimensions));
	gl_Position = vec4(pos - float(Dimensions) * 0.5, 1);
}
