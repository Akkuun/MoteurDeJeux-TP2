#version 330 core

in vec2 UV;


out vec3 color;

uniform sampler2D textureHeightMap;
uniform sampler2D textureRock;
uniform sampler2D textureSnow;

void main() {
    vec3 heightMapColor = texture(textureHeightMap, UV).rgb;
    vec3 rockColor = texture(textureRock, UV).rgb;
    vec3 snowColor = texture(textureSnow, UV).rgb;
    //without mix

    //grey color
    color  = (heightMapColor + rockColor + snowColor) / 3.0;


}