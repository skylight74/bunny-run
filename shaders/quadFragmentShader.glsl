#version 120
#extension GL_EXT_gpu_shader4 : enable
// Inputs from vertex shader
varying vec3 fragPosition;
uniform float groundMovement;

void main() {
    float scale = 0.05;
    float speed = 10.0;
    vec3 offset =  vec3(0.0, -speed*groundMovement, -speed*groundMovement);
    vec3 texCoord = (fragPosition + offset) * scale;
    vec3 colorA = vec3(0.4, 0.4, 0.65); // Dark square color
    vec3 colorB = vec3(0.1, 0.1, 0.1); // Light square color
    int checkX = int(floor(texCoord.x)) % 2;
    int checkZ = int(floor(texCoord.z)) % 2;
    bool isDark = (checkX == 0) == (checkZ == 0);

    vec3 color = isDark ? colorA : colorB;

    gl_FragColor = vec4(color, 1.0);
}

