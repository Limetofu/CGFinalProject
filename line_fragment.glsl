#version 330 core
out vec4 FragColor;

uniform int red;

void main()
{
    if (red == 1) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else {
        FragColor = vec4(1.0); // set alle 4 vector values to 1.0
    }
}