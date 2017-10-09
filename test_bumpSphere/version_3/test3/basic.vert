#version 410 core

//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;
//layout (location = 3) in vec3 aTangent;
//layout (location = 4) in vec3 aBitangent;


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 indices;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

/*
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normalPosition;
layout (location = 3) in vec2 MultiTexCoord;
out vec4 vertexColor;

//-------------
uniform vec3 uColor; // object color
uniform float uSize;
uniform float	Time;		// "Time", from Animate( )
uniform float uCs;
uniform float uA, uB, uC;

uniform int plain;
uniform int vertex_ON;
uniform int frag_ON;
const float PI = 	3.14159265;
const float AMP = 	0.2;		// amplitude
const float W = 	2.;		// frequency

out vec2 vST; // texture coords
out vec2 TexCoord;
out vec4 TotalNewTexCords;
out vec2 getPosition;
//------------
void main()
{

    vec3 vert = position.xyz;
    //getPosition = position.xy;
    getPosition = normalPosition.xy;
    gl_Position = projection_matrix * view_matrix * model_matrix  * vec4( position, 1.0f );
    TexCoord = MultiTexCoord;

    //TotalNewTexCords = vec4(vert, 1.0);
    TotalNewTexCords = vec4(normalPosition, 1.0);
}

*/






