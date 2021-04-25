#version 420 core
layout(location = 0) out vec4 FragColor;

struct Data {
    vec4 color;
    vec2 uv;
    vec3 normal;
    vec3 frag_pos;
};

layout(location = 0) in Data data;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform float materialShininess;
uniform float alpha;
uniform bool textured;
uniform sampler2D textureSample;

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(data.normal);
    vec3 viewDir = normalize(-viewPos - data.frag_pos);
    vec4 color = data.color;
    if (textured) {
        color = texture(textureSample, data.uv);
    }
    vec3 tmp = CalcDirLight(dirLight, norm, viewDir, vec3(color));
    FragColor = vec4(tmp, alpha);
}
