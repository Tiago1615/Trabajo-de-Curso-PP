#version 450 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    Normal = vec3(0.0, 0.0, 1.0);

    gl_Position = uProj * uView * worldPos;
}