#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vertexInfo {
    vec2 oTexCoords;
    vec4 positionDepth;
} vertices[];

out fragmentInfo {
    vec2 oTexCoords;
    mat4 projectionMatrix;
    flat int axis;
    vec4 positionDepth;
} frag;

uniform mat4 ProjX;
uniform mat4 ProjY;
uniform mat4 ProjZ;

void main() {
    vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = normalize(cross(p1, p2));

    int dominantAxis = 0; // Default to x axis
    float maxDot = abs(normal.x);
    
    if (abs(normal.y) > maxDot) {
        dominantAxis = 1; // y axis
        maxDot = abs(normal.y);
    }
    if (abs(normal.z) > maxDot) {
        dominantAxis = 2; // z axis
    }

    // Choose the appropriate projection matrix
    frag.axis = dominantAxis + 1;
    frag.projectionMatrix = (dominantAxis == 0) ? ProjX : (dominantAxis == 1) ? ProjY : ProjZ;

    // Project 3D vertices to 2D plane
    for(int i = 0; i < gl_in.length(); i++) {
        frag.oTexCoords = vertices[i].oTexCoords;
        frag.positionDepth = vertices[i].positionDepth;
        gl_Position = frag.projectionMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }

    // Finished creating vertices
    EndPrimitive();
}
