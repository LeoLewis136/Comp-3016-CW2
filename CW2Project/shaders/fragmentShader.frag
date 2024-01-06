#version 460
//Colour value to send to next stage
out vec4 FragColor;

//Texture coordinates from last stage
in vec2 textureFrag;
in vec3 colourFrag;

uniform sampler2D texture_diffuse1;

void main()
{
    vec3 tempColour
    //Setting of colour coordinates to colour map
    if (textureFrag != vec2(0.0f)){
        tempColour = texture(texture_diffuse1, textureFrag);
    }
    else{
        tempColour = vec4(colourFrag, 1.0f);
    }
}