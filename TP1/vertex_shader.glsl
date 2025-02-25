#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float vertexHeight; // Vérifie que c'est bien déclaré

out vec2 TexCoord;
out float Height;

uniform mat4 MVP;

void main() {
        gl_Position = MVP * vec4(vertexPosition, 1.0);
        TexCoord = texCoord;
        Height = vertexHeight; // Transmettre la hauteur au fragment shader
}
