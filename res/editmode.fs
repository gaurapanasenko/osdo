#version 330 core
out vec4 FragColor;

in vec4 frag_color;
in vec2 frag_uv;
in vec4 frag_pos;
in vec3 frag_min_coord;
in vec3 frag_max_coord;
in vec2 viewPortSize;

void main()
{
    vec4 color = vec4(0.0,1.0,0.0,1.0);
    float minmin = min(frag_min_coord.z, frag_max_coord.z);
    float maxmax = max(frag_min_coord.z, frag_max_coord.z);
    //color.xy = gl_FragCoord.xy - frag_pos.xy / frag_pos.z;
    //color.xy = 2.0 * gl_FragCoord.xy / viewPortSize - 1.0;
    float len = length((frag_pos.xy / frag_pos.z + 1.0) / 2.0 * viewPortSize - gl_FragCoord.xy);
    color.a = 1.0 - len / 10.0;
    if(color.a < 0.5)
        color.xy = vec2(1,0);
    if(color.a < 0.1)
        discard;
    FragColor = color;
}
