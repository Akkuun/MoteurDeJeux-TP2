#version 330 core

layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec2 vertices_uv;

out vec2 UV;
out float vertexHeight;

uniform mat4 MVP;

void main() {
        gl_Position = MVP * vec4(vertices_position_modelspace, 1);
        UV = vertices_uv;
        vertexHeight = vertices_position_modelspace.z;
}