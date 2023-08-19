#version 430 core

layout (location = 0) in vec3 iPosition;

uniform mat4 ModelViewProjectionMatrix;

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(iPosition,1.f);
}