#version 410 core

layout (location = 0) out vec4 FragColor;

in VS_OUT
{
    vec4 WorldPosition;
    vec4 WorldNormal;
    vec2 WorldTexCoords;
}
fs_in;

uniform vec3 CameraPosition;
uniform vec3 LightPosition;

uniform sampler2D Texture0;

void main()
{
    const vec3 Diffuse = vec3(1, 0.8, 1);
    const vec3 Specular = vec3(1, 0.8, 1);

    vec3 s = normalize(LightPosition - fs_in.WorldPosition.xyz);
    vec3 v = normalize(CameraPosition - fs_in.WorldPosition.xyz);
    vec3 h = normalize(s + v);

    float f = max(dot(s, fs_in.WorldNormal.xyz), 0.0);
    float d = pow(max(dot(h, fs_in.WorldNormal.xyz), 0.0), 5.6);

    FragColor = vec4((Diffuse * f + Specular * d) * texture(Texture0, fs_in.WorldTexCoords).rgb, 1.0);
}
