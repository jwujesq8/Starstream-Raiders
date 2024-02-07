#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


vec3 bColor;
const float PI = 3.14159265359;
float AMBIENT = 0.1;

in VS_OUT {
    vec3 worldPos;
    vec3 vecNormal;
    vec2 texCoords;
    vec3 viewDirTS;
    vec3 lightDirTS;
} fs_in;

//struct Light {
//    vec3 Position;
//    vec3 Color;
//};

//uniform Light lights[4];

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float additionalMetallic;
uniform float additionalRoughness;

uniform sampler2D baseColor;
uniform sampler2D normalMap;
uniform sampler2D rougMap;
uniform sampler2D metalMap;
//uniform vec3 viewPos;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

vec3 F(vec3 h, vec3 v, vec3 F0){
 return F0 + (1.0 - F0) * pow(clamp(1.0 - max(dot(h, v), 0.0), 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


void main()
{           

	//vec3 albedoTex = texture2D(textureAlbedo, fragTexCoord).rgb;
    vec3 bColor = texture(baseColor, fs_in.texCoords).rgb;
	vec3 normalTex= texture2D(normalMap, fs_in.texCoords).rgb; // = vec3(0,0,1)
	float metallic = texture2D(metalMap, fs_in.texCoords).r + additionalMetallic;
	float roughness = texture2D(rougMap, fs_in.texCoords).r + additionalRoughness;
   

    vec3 Lo = vec3(0,0,0);
    //vec3 normal = normalize(fs_in.Normal);
	vec3 N = (texture2D(normalMap,  fs_in.texCoords).rgb * 2) - 1;

    vec3 V = normalize(fs_in.viewDirTS);
	
	
	vec3 L = normalize(fs_in.lightDirTS);
		
	vec3 H = normalize(L + V);
	vec3 attenuatedlightColor = lightColor/pow(length(lightPos-fs_in.worldPos),2); //radial attenuation

	vec3 F0 = vec3(0.04); 
	F0      = mix(F0, bColor, metallic);
	vec3 F  = F(H, V, F0);
	float NDF = DistributionGGX(N, H, roughness);
	float G   = GeometrySmith(N, V, L, roughness);

	vec3 numerator    = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
	vec3 specular     = numerator / denominator;  

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
		
	kD *= 1.0 - 0.5;	

		
  
	float NdotL = max(dot(N, L), 0.0);        
	Lo += (kD * bColor / PI + specular) * attenuatedlightColor * NdotL;

	
	vec3 ambient = vec3(0.03) * bColor;
	vec3 color   = ambient + Lo;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

   
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(color, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(color, 1.0);
}