#version 330 core

in vec2 UV;
in float vertexHeight;

out vec3 color;

uniform sampler2D grassTextureSampler;
uniform sampler2D rockTextureSampler;
uniform sampler2D snowTextureSampler;

void main() {
        vec3 grassColor = texture(grassTextureSampler, UV).rgb;
        vec3 rockColor = texture(rockTextureSampler, UV).rgb;
        vec3 snowColor = texture(snowTextureSampler, UV).rgb;

        float height = vertexHeight;

        vec3 finalColor;
        if (height < 0.3) {
                finalColor = grassColor;
        } else if (height < 0.6) {
                finalColor = mix(grassColor, rockColor, (height - 0.3) / 0.3);
        } else {
                finalColor = mix(rockColor, snowColor, (height - 0.6) / 0.4);
        }

        color = finalColor;
}