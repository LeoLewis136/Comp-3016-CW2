#version 460
//Colour value to send to next stage
out vec4 FragColor;

//Texture coordinates from last stage
in vec2 textureFrag;
in vec3 colourFrag;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightColour;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;

void main()
{
    vec4 tempColour;
    //Setting of colour coordinates to colour map
    if (textureFrag != vec2(0.0f)){
        tempColour = texture(texture_diffuse1, textureFrag);
    }
    else{
        tempColour = vec4(colourFrag, 1.0f);
    }

    // ---- Lighting Calculations ----
    // -- Diffuse -- 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    // -- Ambient --
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColour;

    
    vec4 result = vec4((ambient + diffuse), 1.0) * tempColour;
    FragColor = result;
}