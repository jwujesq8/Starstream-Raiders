#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out VS_OUT {
    vec3 worldPos;
    vec3 vecNormal;
    vec2 texCoords;
    vec3 viewDirTS;
    vec3 lightDirTS;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
    vs_out.worldPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.texCoords = aTexCoords;
        
    //mat3 normalMatrix = transpose(inverse(mat3(model)));
   // vs_out.Normal = normalize(normalMatrix * aNormal);
    vs_out.vecNormal = normalize((model * vec4(aNormal,0)).xyz);
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec3 w_tangent = normalize(mat3(model)*vertexTangent);
	vec3 w_bitangent = normalize(mat3(model)*vertexBitangent);
	mat3 TBN = transpose(mat3(w_tangent, w_bitangent, vs_out.vecNormal));
	
	vec3 V = normalize(cameraPos-vs_out.worldPos);
	vs_out.viewDirTS = TBN*V;
	vec3 L = normalize(lightPos-vs_out.worldPos);
	vs_out.lightDirTS = TBN*L;
}