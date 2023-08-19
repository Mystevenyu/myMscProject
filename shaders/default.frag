#version 430 core

// Interpolated values from the vertex shaders

in vec3 oPosition;
in vec2 oTexCoords;
in vec3 oNormal;
in vec3 oTangent;
in vec3 oBitangent;
in vec3 oCameraDir;
in vec4 LightSpacePosition; // Position from the shadow map point of view

out vec4 color;


// Textures
uniform sampler2D uniformDiffuse;
uniform sampler2D uniformSpecular;

uniform sampler2D HeightTexture;
uniform vec2 HeightTextureSize;


// Shadow map
uniform sampler2DShadow ShadowMap;

// Voxel stuff
uniform sampler3D VoxelTexture;
uniform float VoxelGridWorldSize;
uniform int VoxelDimensions;

// Toggle "booleans"
struct LightingParameters {
    float ShowDiffuse;
    float ShowIndirectDiffuse;
    float ShowIndirectSpecular;
    float ShowAmbientOcculision;
};

uniform LightingParameters lighting;


uniform vec3 LightDirection;

const float MAX_DIST = 100.0;

// // 5 90 degree cones
 const int NUM_CONES = 5;
 vec3 coneDirections[5] = vec3[](vec3(0, 1, 0),vec3(0, 0.707, 0.707),vec3(0, 0.707, -0.707),vec3(0.707, 0.707, 0),vec3(-0.707, 0.707, 0));
 float coneWeights[5] = float[](0.28, 0.18, 0.18, 0.18, 0.18);

mat3 tangentToWorld;

vec4 VoelsSampling(vec3 worldPosition, float lod) {
    vec3 offset = vec3(2.0 / VoxelDimensions, 2.0 / VoxelDimensions, 0); 
    vec3 voxelTextureUV = worldPosition / (VoxelGridWorldSize * 0.5);
    voxelTextureUV = voxelTextureUV * 0.5 + 0.5 + offset;
    return textureLod(VoxelTexture, voxelTextureUV, lod);
}

vec4 RayTracingWithCone(vec3 direction, float tanHalfAngle, out float occlusion) 
{
    vec3 color = vec3(0);
    float alpha = 0.0;
    occlusion = 0.0;

    float voxelWorldSize = VoxelGridWorldSize / VoxelDimensions;
    float dist = voxelWorldSize; // Start one voxel away to avoid self occlusion
    vec3 startPos = oPosition + oNormal * voxelWorldSize; // Plus move away slightly in the normal direction to avoid
                                                                    // self occlusion in flat surfaces

    while(dist < MAX_DIST && alpha < 0.95f) {
        // smallest sample diameter possible is the voxel size
        float diameter = max(voxelWorldSize, 2.0 * tanHalfAngle * dist);
        float lodLevel = log2(diameter / voxelWorldSize);
        vec4 voxelColor = VoelsSampling(startPos + dist * direction, lodLevel);

        // front-to-back compositing
        float a = (1.0 - alpha);
        color += a * voxelColor.rgb;
        alpha += a * voxelColor.a;
        //occlusion += a * voxelColor.a;
        occlusion += (a * voxelColor.a) / (1.0 + 0.03 * diameter);
        dist += diameter; 
    }

    return vec4(color, alpha);
}

vec4 CalculateIndirectLight(out float occlusion_out) 
{
    vec4 color = vec4(0);
    occlusion_out = 0.0;

    for(int i = 0; i < NUM_CONES; i++) {
        float occlusion = 0.0;

        color += coneWeights[i] * RayTracingWithCone(tangentToWorld * coneDirections[i], 0.577, occlusion);
        occlusion_out += coneWeights[i] * occlusion;
    }

    occlusion_out = 1.0 - occlusion_out;

    return color;
}

vec3 CalculateBumpNormal() 
{
    // Calculate gradients
    vec2 offset = vec2(1.0) / HeightTextureSize;
    float curr = texture(HeightTexture, oTexCoords).r;
    float diffX = texture(HeightTexture, oTexCoords + vec2(offset.x, 0.0)).r - curr;
    float diffY = texture(HeightTexture, oTexCoords + vec2(0.0, offset.y)).r - curr;

    // Tangent space bump normal
    float bumpMult = -3.0;
    vec3 bumpNormal_tangent = normalize(vec3(bumpMult*diffX, 1.0, bumpMult*diffY));

    return normalize(tangentToWorld * bumpNormal_tangent);
}

vec3 CalculateDiffuseReflection(in vec3 N, in vec3 L, in vec3 E, in vec3 materialColor) {
    // Shadow map
    float visibility = texture(ShadowMap, vec3(LightSpacePosition.xy, (LightSpacePosition.z - 0.0005)/LightSpacePosition.w));

    // Direct diffuse light
    float cosTheta = max(0, dot(N, L));
    vec3 directDiffuseLight = lighting.ShowDiffuse > 0.5 ? vec3(visibility * cosTheta) : vec3(0.0);

    // Indirect diffuse light
    float occlusion = 0.0;
    vec3 indirectDiffuseLight = CalculateIndirectLight(occlusion).rgb;
    indirectDiffuseLight = lighting.ShowIndirectDiffuse > 0.5 ? 4.0 * indirectDiffuseLight : vec3(0.0);

    // Sum direct and indirect diffuse light and tweak a little bit
    occlusion = min(1.0, 1.5 * occlusion); // Make occlusion brighter
    return 2.0 * occlusion * (directDiffuseLight + indirectDiffuseLight) * materialColor;
}

vec3 CalculateSpecularReflection(in vec3 N, in vec3 E, in vec4 specularColor) {
    // Some specular textures are grayscale:
    specularColor = length(specularColor.gb) > 0.0 ? specularColor : specularColor.rrra;
    vec3 reflectDir = normalize(-E - 2.0 * dot(-E, N) * N);

    // Maybe fix so that the cone doesnt trace below the plane defined by the surface normal.
    // For example so that the floor doesnt reflect itself when looking at it with a small angle
    float specularOcclusion;
    vec4 tracedSpecular = RayTracingWithCone(reflectDir, 0.07, specularOcclusion); // 0.2 = 22.6 degrees, 0.1 = 11.4 degrees, 0.07 = 8 degrees angle
    return lighting.ShowIndirectSpecular > 0.5 ? 2.0 * specularColor.rgb * tracedSpecular.rgb : vec3(0.0);
}

void main() {
    vec4 materialColor = texture(uniformDiffuse, oTexCoords);
    float alpha = materialColor.a;

    if(alpha < 0.5) {
        discard;
    }
    
    tangentToWorld = inverse(transpose(mat3(oTangent, oNormal, oBitangent)));

    // Normal, light direction and eye direction in world coordinates
    vec3 N = CalculateBumpNormal();
    vec3 L = LightDirection;
    vec3 E = normalize(oCameraDir);

    vec3 diffuseReflection = CalculateDiffuseReflection(N, L, E, materialColor.rgb);
    vec4 specularColor = texture(uniformSpecular, oTexCoords);
    vec3 specularReflection = CalculateSpecularReflection(N, E, specularColor);

    color = vec4(diffuseReflection + specularReflection, alpha);
}
