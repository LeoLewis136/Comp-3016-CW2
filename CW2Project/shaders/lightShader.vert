#version 460
// Vertex positions
layout (location = 0) in vec3 position;

// Model-View-Projection Matrix
uniform mat4 mvpIn;
uniform vec3 lightColour;

out vec3 colourFrag;

void main()
{
    //Transformation applied to vertices
    gl_Position = mvpIn * vec4(position.x, position.y, position.z, 1.0);

    colourFrag = lightColour;
}