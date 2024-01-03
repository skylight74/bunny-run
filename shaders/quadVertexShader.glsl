#version 120

// Uniform transformation matrices
uniform mat4 modelingMat;
uniform mat4 viewingMat;
uniform mat4 perspectiveMat;

// Vertex attributes
attribute vec3 inVertex;

// Outputs to the fragment shader
varying vec3 fragPosition;

void main() {
    // Transform vertex position to clip space
    gl_Position = perspectiveMat * viewingMat* modelingMat* vec4(inVertex, 1.0);

    // Pass the world position to the fragment shader
    fragPosition = (modelingMat * vec4(inVertex, 1.0)).xyz;
}

