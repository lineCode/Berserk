#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

struct LightInfo
{
    vec4 Position;      // light position in view coordinates
    vec3 La;            // Light ambient component
    vec3 Ld;            // Light diffuse component
    vec3 Ls;            // Light specular component
};

uniform LightInfo Light;

struct MaterialInfo
{
    vec3 Ka;            // ambient component
    vec3 Kd;            // diffuse component
    vec3 Ks;            // specular component
    float Shininess;    // exponent of specular component
};

uniform MaterialInfo Material;

uniform mat4 ModelView;
uniform mat4 MVP;

vec3 ads(vec4 position, vec3 norm)
{
    vec3 s = normalize(vec3(Light.Position - position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect(-s, norm);

    float sDotn = max(dot(s, norm), 0.0);

    vec3 ambient = Light.La * Material.Ka;
    vec3 diffuse = Light.Ld * Material.Kd * sDotn;

    vec3 specular = vec3(0.0);

    if (sDotn > 0.0)
    {
        specular = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
    }

    return ambient + diffuse + specular;
}

void main()
{
    vec3 eyeNorm = normalize((ModelView * vec4(VertexNormal, 0.0)).xyz);
    vec4 eyePosition = ModelView * vec4(VertexPosition, 1.0);

    LightIntensity = ads(eyePosition, eyeNorm);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}