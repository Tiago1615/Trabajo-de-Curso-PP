#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 uColor;
uniform vec3 uLightDir;
uniform vec3 uViewPos;

void main()
{
    // Normalizar
    vec3 N = normalize(Normal);
    vec3 L = normalize(-uLightDir);
    vec3 V = normalize(uViewPos - FragPos);

    // Ambiente
    vec3 ambient = 0.2 * uColor;

    // Difusa
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * uColor;

    // Especular (Phong)
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), 16.0);
    vec3 specular = vec3(0.3) * spec;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}