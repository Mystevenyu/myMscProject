#version 430
#extension GL_ARB_shader_image_load_store : enable

// Data from geometry shader
in fragmentInfo {
    vec2 oTexCoords;
    mat4 projectionMatrix;
    flat int axis;
    vec4 positionDepth;
} frag;

uniform layout(RGBA8) image3D VoxelTexture;
uniform sampler2D uniformDiffuse;
uniform sampler2DShadow ShadowMap;
uniform int VoxelDimensions;

void main() {
    vec4 materialColor = texture(uniformDiffuse, frag.oTexCoords);
    float visibility = texture(ShadowMap, vec3(frag.positionDepth.xy, (frag.positionDepth.z - 0.001)/frag.positionDepth.w));

	ivec3 camPos = ivec3(gl_FragCoord.x, gl_FragCoord.y, VoxelDimensions * gl_FragCoord.z);
	ivec3 texPos;

	switch (frag.axis) {
		case 1:
		    texPos = ivec3(VoxelDimensions - camPos.z, camPos.y, camPos.x);
			break;
		case 2:
		    texPos = ivec3(camPos.x, VoxelDimensions - camPos.z, camPos.y);
			break;
		default:
		    texPos = camPos;
			break;
	}

	// Flip z value
	texPos.z = VoxelDimensions - texPos.z - 1;

    imageStore(VoxelTexture, texPos, vec4(materialColor.rgb * visibility, 1.0));
}
