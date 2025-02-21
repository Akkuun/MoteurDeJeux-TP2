#version 330 core

in vec2 UV;
in float vertexHeight;

out vec3 color;

uniform sampler2D grassTextureSampler;
uniform sampler2D rockTextureSampler;
uniform sampler2D snowTextureSampler;

void main() {
        if (vertexHeight < 0.3) {
                color = texture(grassTextureSampler, UV).rgb;
        } else if (vertexHeight < 0.6) {
                color = texture(rockTextureSampler, UV).rgb;
        } else {
                color = texture(snowTextureSampler, UV).rgb;
        }
}