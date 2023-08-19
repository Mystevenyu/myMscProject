#version 430 core

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTexCoords;
layout(location = 2) in vec3 iNormal;
layout(location = 3) in vec3 iTangent;
layout(location = 4) in vec3 iBitangent;

out vec3 oPosition;
out vec2 oTexCoords;
out vec3 oNormal;
out vec3 oTangent;
out vec3 oBitangent;
out vec3 oCameraDir;

out vec4 LightSpacePosition;

uniform vec3 CameraPosition;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 LightModelViewProjectionMatrix;

void main() {
    vec4 worldPosition = ModelMatrix * vec4(iPosition, 1);
    oPosition = worldPosition.xyz;

    LightSpacePosition = LightModelViewProjectionMatrix * vec4(iPosition, 1);
    LightSpacePosition.xyz = LightSpacePosition.xyz * 0.5 + 0.5;

    oNormal = normalize((ModelMatrix * vec4(iNormal, 0)).xyz);
    oTangent = normalize((ModelMatrix * vec4(iTangent, 0)).xyz);
    oBitangent = normalize((ModelMatrix * vec4(iBitangent, 0)).xyz);

    oCameraDir = CameraPosition - oPosition;
    oTexCoords = iTexCoords;

    gl_Position = ProjectionMatrix * ViewMatrix * worldPosition;
}
