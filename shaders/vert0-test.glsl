#version 120

attribute vec3 inVertex; // Assuming you are using 'inVertex' to pass vertex positions

uniform mat4 perspectiveMat; // You still need this for basic perspective projection
uniform mat4 modelingMat;    // And this to place your model in the world

void main(void) {
    gl_Position = perspectiveMat * modelingMat * vec4(inVertex, 1.0);
}

