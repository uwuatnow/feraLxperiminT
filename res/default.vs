#version 120

attribute vec3 aPosition; // Vertex position input

void main()
{
    gl_Position = vec4(aPosition, 1.0); // Pass input position to output
}