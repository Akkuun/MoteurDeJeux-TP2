#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 inUV;
layout(location = 2) in float inVertexHeight;

out vec2 UV;
out float vertexHeight;

uniform mat4 MVP;

void main() {
        gl_Position = MVP * vec4(position, 1.0);
        UV = inUV;
        vertexHeight = inVertexHeight;
}