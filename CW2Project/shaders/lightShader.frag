#version 460

in vec3 colourFrag;

out vec4 FragColor;

void main(){
    FragColor = vec4(colourFrag, 1.0);
}