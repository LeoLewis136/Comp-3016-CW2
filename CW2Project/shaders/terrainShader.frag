#version 460
//Colour value to send to next stage
out vec4 FragColor;

//Texture coordinates from last stage
in vec3 colourFrag;
in vec3 Normal;
in vec3 FragPos;
in vec3 viewPos;

// Light properties
uniform vec3 lightColour;
uniform vec3 lightPos;

void main()
{
    vec4 tempColour;

    //Setting of colour values to the biome data
    tempColour = vec4(colourFrag, 1.0f);


    // ---- Lighting Calculations ----
    // -- Diffuse -- 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColour;

    // -- Ambient --
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColour;


    // -- Specular --
    float specularStrength = 0.01;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColour; 

    // Applying the colours
    vec4 result = vec4((ambient + diffuse + specular), 1.0) * tempColour;
    FragColor = result;
}