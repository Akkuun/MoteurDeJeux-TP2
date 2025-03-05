#version 330 core

in vec2 UV;


out vec3 color;


uniform sampler2D textureRock;


void main() {
    vec3 rockColor = texture(textureRock, UV).rgb;
    //without mix

    //grey color
    color  = rockColor;


}