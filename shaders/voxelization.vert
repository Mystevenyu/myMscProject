#version 430 core

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTexCoords;


out vertexInfo {
    vec2 oTexCoords;
    vec4 positionDepth;
} vert;

uniform mat4 LightModelViewProjectionMatrix;
uniform mat4 ModelMatrix;



void main() {
    vert.oTexCoords = iTexCoords;
    vert.positionDepth = LightModelViewProjectionMatrix * vec4(iPosition, 1);
    vert.positionDepth.xyz = vert.positionDepth.xyz / 2.0f + 0.5f;

    gl_Position = ModelMatrix * vec4(iPosition, 1);
}
