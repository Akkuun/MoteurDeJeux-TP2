#version 330 core

in vec2 UV;
in float vertexHeight;

out vec4 color;

uniform sampler2D grassTextureSampler;
uniform sampler2D rockTextureSampler;
uniform sampler2D snowTextureSampler;

void main() {
    vec4 grassColor = texture(grassTextureSampler, UV);
    vec4 rockColor = texture(rockTextureSampler, UV);
    vec4 snowColor = texture(snowTextureSampler, UV);

    if (vertexHeight < 0.33) {
        color = grassColor;
    } else if (vertexHeight < 0.66) {
        color = rockColor;
    } else {
        color = snowColor;
    }
}