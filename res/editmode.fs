#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 frag_color;
in vec2 frag_uv;
in vec3 frag_pos;
in vec4 frag_pos2;
in vec3 frag_min_coord;
in vec3 frag_max_coord;
in vec2 viewPortSize;
in vec3 frag_normal;

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
    vec3 norm = normalize(frag_normal);
    vec3 viewDir = normalize(viewPos - frag_pos);
    vec4 color = vec4(0.0,1.0,0.0,1.0);
    //float minmin = min(frag_min_coord.z, frag_max_coord.z);
    //float maxmax = max(frag_min_coord.z, frag_max_coord.z);
    //color.xy = gl_FragCoord.xy - frag_pos2.xy / frag_pos2.z;
    //color.xy = 2.0 * gl_FragCoord.xy / viewPortSize - 1.0;
    //float len = length((frag_pos2.xy / frag_pos2.z + 1.0) / 2.0 * viewPortSize - gl_FragCoord.xy);
    //color.a = 1.0 - len / 10.0;
    if(color.a < 0.5)
        color.xy = vec2(1,0);
    //if(color.a < 0.1)
        //discard;
    vec3 tmp = CalcDirLight(dirLight, norm, viewDir, vec3(0,1.0,0));
    FragColor = vec4(tmp, alpha);
}
