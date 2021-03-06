#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 eyeNorm;
out vec4 eyePosition;

uniform mat4 ModelView;
uniform mat4 MVP;

void main()
{
    eyeNorm = normalize((ModelView * vec4(VertexNormal, 0.0)).xyz);
    eyePosition = ModelView * vec4(VertexPosition, 1.0);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}