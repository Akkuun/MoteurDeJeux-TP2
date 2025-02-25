#version 330 core

in vec2 UV;
in float height;

out vec3 color;

uniform sampler2D textureHeightMap;
uniform sampler2D textureRock;
uniform sampler2D textureSnow;

void main() {
    vec3 heightMapColor = texture(textureHeightMap, UV).rgb;
    vec3 rockColor = texture(textureRock, UV).rgb;
    vec3 snowColor = texture(textureSnow, UV).rgb;

    //without mix

    if(height < 0.33){
        color = heightMapColor;
    }else if(height < 0.66){
        color = rockColor;
    }
    else{
        color = snowColor;
    }

//    // Mix textures based on height
//    if (height < 0.33) {
//        color = mix(heightMapColor, rockColor, height / 0.33);
//    } else if (height < 0.66) {
//        color = mix(rockColor, snowColor, (height - 0.33) / 0.33);
//    } else {
//        color = snowColor;
//    }
}