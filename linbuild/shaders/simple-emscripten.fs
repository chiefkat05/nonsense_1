#version 300 es
out highp vec4 FragColor;

uniform highp vec4 color; // the input variable from the vertex shader (same name and same type)  

void main()
{
    FragColor = color;
} 