#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    float Roughness; 
    float Metallic;
    vec3 color;
    bool HasDiffuse;
    sampler2D Diffuse;        
};


struct Light {
    int type;            // 0 = Directional, 1 = Point, 2 = Spot
    vec3 position;       
    vec3 direction;      
    vec3 color;          
    float intensity;     
    float range;         
    float innerCutoff;   
    float outerCutoff;   
};

#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

uniform Material material; 
uniform vec3 camPos;
uniform samplerCube skybox;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// PBR FUNCTIONS
// ----------------------------------------------------------------------------

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------

void main()
{
    // 1. Material Setup
    vec3 albedo = material.color;
    if (material.HasDiffuse) {
        // Gamma Correct the texture (sRGB -> Linear)
        vec3 texColor = texture(material.Diffuse, TexCoords).rgb;
        albedo *= pow(texColor, vec3(2.2)); 
    }

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - FragPos);
    vec3 R = reflect(-V, N); 

    // Safety Clamps
    float roughness = clamp(material.Roughness, 0.05, 1.0);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, material.Metallic);

    // ------------------------------------------------------------------------
    // LIGHTING LOOP
    // ------------------------------------------------------------------------
    vec3 Lo = vec3(0.0); // Accumulator for all lights

    for(int i = 0; i < lightCount; ++i) 
    {
        // A. Calculate Light Vector (L) and Attenuation
        vec3 L;
        float attenuation = 1.0;

        if (lights[i].type == 0) // Directional Light (Sun)
        {
            L = normalize(-lights[i].direction); // Direction comes from Transform.Forward
            attenuation = 1.0; // Sun doesn't fade with distance
        }
        else // Point (1) or Spot (2)
        {
            vec3 lightVec = lights[i].position - FragPos;
            float distance = length(lightVec);
            L = normalize(lightVec);
            
            // 1. Basic Physics (Inverse Square Law)
            // We add 1.0 to distance or use max() to prevent divide-by-zero at the exact center
            float attenuation = 1.0 / (distance * distance + 1.0); 

            // 2. The Windowing Function (Smoothly force to 0.0 at range)
            // This matches the standard used by Unreal Engine and Frostbite
            float lightRadius = lights[i].range;
            float distSqr = distance * distance;
            float radiusSqr = lightRadius * lightRadius;
            
            // Calculate factor: 0.0 at center, 1.0 at radius
            float factor = distSqr / radiusSqr;
            
            // Invert and clamp: 1.0 at center, 0.0 at radius
            float smoothFactor = max(0.0, 1.0 - factor * factor);
            smoothFactor = smoothFactor * smoothFactor;

            // 3. Final Attenuation
            attenuation = attenuation * smoothFactor;
        }

        // B. Spot Light Cone Logic
        if (lights[i].type == 2) // Spot Light
        {
            float theta = dot(L, normalize(-lights[i].direction)); 
            float epsilon = lights[i].innerCutoff - lights[i].outerCutoff;
            float intensity = clamp((theta - lights[i].outerCutoff) / epsilon, 0.0, 1.0);
            attenuation *= intensity;
        }

        // C. Calculate Radiance
        vec3 radiance = lights[i].color * lights[i].intensity * attenuation;

        // D. Cook-Torrance BRDF
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0); 

        // Optimization: Skip calculation if light is behind the surface
        if (NdotL > 0.0) 
        {
            float D = DistributionGGX(N, H, roughness);   
            float G = GeometrySmith(N, V, L, roughness);      
            vec3  F = FresnelSchlick(max(dot(H, V), 0.0), F0);
            
            vec3 numerator    = D * G * F; 
            float NdotV       = max(dot(N, V), 0.0);
            float denominator = 4.0 * NdotV * NdotL + 0.0001; 
            vec3 specular     = numerator / denominator;
            
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - material.Metallic;	  
            
            // Add to accumulator
            Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        }
    }

    // ------------------------------------------------------------------------
    // IBL (Ambient)
    // ------------------------------------------------------------------------
    float NdotV = max(dot(N, V), 0.0);
    vec3 F_IBL = FresnelSchlickRoughness(NdotV, F0,roughness);
    vec3 kS_IBL = F_IBL;
    vec3 kD_IBL = 1.0 - kS_IBL;
    kD_IBL *= (1.0 - material.Metallic);
    
    vec3 diffuseIBL = kD_IBL * albedo * 0.03; // Base Ambient Strength
    
    // Specular IBL (Skybox Reflection)
    float lodLevel = roughness * 4.0;
    float horizonOcclusion = 1.0 - roughness;
    vec3 specularIBL = textureLod(skybox, R, lodLevel).rgb * kS_IBL * horizonOcclusion;

    vec3 Ambient = diffuseIBL + specularIBL;

    // ------------------------------------------------------------------------
    // FINAL COMPOSITION
    // ------------------------------------------------------------------------
    vec3 color = Ambient + Lo;

    // Tone Mapping (Reinhard)
    color = color / (color + vec3(1.0));
    // Gamma Correction
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}