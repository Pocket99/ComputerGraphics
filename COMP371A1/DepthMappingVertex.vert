#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 light_view_proj_matrix;
uniform mat4 modelMat;

void main() {
    gl_Position = light_view_proj_matrix * modelMat * vec4(aPos, 1.0);
}