#version 400 core
out vec4 fragColor;
  
in vec3 color;

uniform sampler2D texture[16];

void main()
{
    fragColor = vec4(color, 1.0);
}