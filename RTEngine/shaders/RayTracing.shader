###SHADER(VERTEX)
#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
}

###SHADER(FRAGMENT)
#version 430 core
#define lerp mix
#define LOWETS_THRESHOLD 1.0e-6F
#define FLT_MAX 3.402823466e+38F
#define UINT_MAX 4294967295.0
#define PI 3.141592653589793
in vec2 TexCoords;

layout (location = 0) out vec4 AccumulationColor;
layout (location = 1) out vec4 ScreenColor;

uniform sampler2D u_AccumulationTexture;
uniform sampler2D u_ScreenTexture;

uniform float u_DrawEnvironment;
uniform uint u_MaxBounces;
uniform uint u_MaxFrames;
uniform uint u_FrameIndex;
uniform vec2 u_Resolution;

layout(std430, binding = 0) buffer Camera
{
    mat4 Projection;
    mat4 View;
    vec3 Position;
} b_Camera;

struct Material
{
    vec3 Albedo;
    vec3 EmmisionColor;
    float Roughness;
    float Metalic;
    float EmmisionPower;
};

uniform int u_MaterialsCount;
layout(std430, binding = 1) buffer MaterialsBuffer
{
    Material b_Materials[];
};

vec3 GetEmmision(in int matIndex)
{
    return b_Materials[matIndex].EmmisionColor * b_Materials[matIndex].EmmisionPower;
}

struct Sphere
{
    vec3 Position;
    float Radius;
    int MaterialId;
};

uniform int u_SpheresCount;
layout(std430, binding = 2) buffer SpheresBuffer
{
    Sphere b_Spheres[];
};

uint pcg_hash(in uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float FastRandom(inout uint seed)
{
    seed = pcg_hash(seed);
    //float theta = 2.0 * PI * float(seed) / UINT_MAX;
    //seed = pcg_hash(seed);
    //float rho = sqrt(-2.0 * log(float(seed) / UINT_MAX));
    //return rho * cos(theta);
    return float(seed) / UINT_MAX;
}

vec3 RandomUnitSpehere(inout uint seed)
{
    return 2.0 * vec3(FastRandom(seed), FastRandom(seed), FastRandom(seed)) - 1.0;
}

struct Ray
{
    vec3 Origin;
    vec3 Direction;
};

struct Payload
{
    vec3 Color;
    vec3 HitPosition;
    vec3 HitNormal;
    float HitDistance;
    int HitObject;
};

const vec3 LightDir = normalize(vec3(-1, -1, -1));
const vec3 GroundColor = vec3(0.3);
const vec3 SkyColorZenith = vec3(0.5, 0.7, 1.0);
const vec3 SkyColorHorizon = vec3(0.6, 0.4, 0.4);

Payload Miss(in Ray ray)
{
    //vec3 skyColor = vec3((1.0 - ray.Direction.yy) / 4.0 + 0.6, 1.0);
    float skyLerp = pow(smoothstep(0.0, 0.4, ray.Direction.y), 0.35);
    float groundToSky = smoothstep(-0.01, 0.0, ray.Direction.y);
    vec3 skyGradient = lerp(SkyColorHorizon, SkyColorZenith, skyLerp);
    float sun = pow(max(0.0, dot(ray.Direction, -LightDir)), 500.0) * 100.0;
    vec3 skyColor = lerp(GroundColor, skyGradient, groundToSky) + sun * float(groundToSky >= 1.0);
    
    Payload payload;
    payload.Color = skyColor;
    payload.HitPosition = vec3(0);
    payload.HitNormal = vec3(0);
    payload.HitDistance = FLT_MAX;
    payload.HitObject = -1;
    
    return payload;
}


Payload ClosestHit(in Ray ray, in float closestDistance, in int closestObject)
{   
    Payload payload;
    payload.Color = b_Materials[b_Spheres[closestObject].MaterialId].Albedo;
    payload.HitPosition = ray.Origin + closestDistance * ray.Direction;
    payload.HitNormal = normalize(payload.HitPosition - b_Spheres[closestObject].Position);
    payload.HitDistance = closestDistance;
    payload.HitObject = closestObject;
        
    return payload;
}

Payload TraceRay(in Ray ray)
{
    float closestDistance = FLT_MAX;
    int closestObject = -1;
    
    for (int sphereId = 0; sphereId < u_SpheresCount; sphereId++)
    {
        vec3 origin = ray.Origin - b_Spheres[sphereId].Position;
        
        float a = dot(ray.Direction, ray.Direction);
        float b = 2.0 * dot(origin, ray.Direction);
        float c = dot(origin, origin) - pow(b_Spheres[sphereId].Radius, 2.0);
    
        float delta = b * b - 4.0 * a * c;
    
        if (delta < 0.0)
            continue;
        
        float t = (-b - sqrt(delta)) / (2.0 * a);
        if (t >= 0.0 && t < closestDistance)
        {
            closestDistance = t;
            closestObject = sphereId;
        }
    }
    
    if (closestObject == -1)
        return Miss(ray);
    
    return ClosestHit(ray, closestDistance, closestObject);
}

vec3 calculateBRDF(in vec3 Direction, in vec3 HitNormal, in int HitObject)
{
    
            
    float lightIntensity = max(dot(HitNormal, -LightDir), 0.0);
    vec3 viewDir = -normalize(Direction);
            
    // // Diffuse
    vec3 lambert = b_Materials[b_Spheres[HitObject].MaterialId].Albedo / PI;
    // //
            
    // // Specular
    // Normal Distribution Function
    vec3 halfWay = normalize(viewDir - LightDir);
    float alpha2 = b_Materials[b_Spheres[HitObject].MaterialId].Roughness;
    alpha2 = alpha2 * alpha2 * alpha2 * alpha2;
            
    float NdotH = max(dot(HitNormal, halfWay), 0.0);
            
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
            
    float normalDistributionGGX = alpha2 / (PI * denom * denom);
    //
    
    // Geometry Function
    float k = alpha2 / 2.0;
            
    float NdotV = max(dot(HitNormal, viewDir), 0.0);
    float SchlickGGX1 = NdotV / max((NdotV * (1 - k) + k), LOWETS_THRESHOLD);
            
    float NdotL = max(dot(HitNormal, -LightDir), 0.0);
    float SchlickGGX2 = NdotL / max((NdotL * (1 - k) + k), LOWETS_THRESHOLD);
            
    float geometrySmith = SchlickGGX1 * SchlickGGX2;
    //
    
    // Frasnel Equation
    vec3 F0 = vec3(0.04);
    F0 = mix(
        F0,
        b_Materials[b_Spheres[HitObject].MaterialId].Albedo,
        b_Materials[b_Spheres[HitObject].MaterialId].Metalic
    );
    vec3 frasnelSchlick = F0 + (1.0 - F0) * pow(1.0 - lightIntensity, 5.0);
    //
            
    vec3 Ks = frasnelSchlick;
    vec3 Kd = (1.0 - Ks) * (1.0 - b_Materials[b_Spheres[HitObject].MaterialId].Metalic);
    vec3 cookTorranceNumerator = normalDistributionGGX * frasnelSchlick * geometrySmith;
    float cookTorranceDenumerator = max(4.0 * max(dot(viewDir, HitNormal), 0.0) * max(dot(-LightDir, HitNormal), 0.0), LOWETS_THRESHOLD);
    vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenumerator;
            
    vec3 BRDF = Kd * lambert + cookTorrance;
    // //
            
    return BRDF * max(dot(-LightDir, HitNormal), 0.0); 
}

void main()
{   
    vec3 finalColor = vec3(0);
    
    vec2 pixel = gl_FragCoord.xy / u_Resolution;
    vec4 coord = b_Camera.Projection * (2.0 * vec4(pixel, 1.0, 1.0) - 1.0);
    
    Ray precalculatedRay;
    precalculatedRay.Origin = b_Camera.Position;
    precalculatedRay.Direction = vec3(b_Camera.View * vec4(normalize(coord.xyz / coord.w), 0));
    
    for (uint frame = 1; frame <= u_MaxFrames; frame++)
    {
        uint seed = uint(gl_FragCoord.y * u_Resolution.x + gl_FragCoord.x) + frame * u_FrameIndex * 735529;
        
        Ray ray = precalculatedRay;
        vec3 contribution = vec3(1);
    
        for (uint i = 0u; i < u_MaxBounces; i++)
        {
            seed += i;
        
            Payload payload = TraceRay(ray);
         
            if (payload.HitObject == -1)
            {
                finalColor += payload.Color * contribution * u_DrawEnvironment;
                break;
            }
            
            finalColor += GetEmmision(b_Spheres[payload.HitObject].MaterialId) * contribution;
            contribution *= b_Materials[b_Spheres[payload.HitObject].MaterialId].Albedo;
            
            //finalColor += contribution * calculateBRDF(ray.Direction, payload.HitNormal, payload.HitObject);
            
            ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001;
        
            vec3 diffuseDir = normalize(payload.HitNormal + RandomUnitSpehere(seed));
            vec3 specularDir = reflect(ray.Direction, payload.HitNormal);
            
            ray.Direction = lerp(diffuseDir, specularDir, b_Materials[b_Spheres[payload.HitObject].MaterialId].Roughness);
            //ray.Direction = normalize(payload.HitNormal + RandomUnitSpehere(seed));
        }
    }
    
    finalColor = finalColor / float(u_MaxFrames);
    
    if (u_FrameIndex != 1)
        finalColor += texture(u_AccumulationTexture, TexCoords).xyz;
        
    AccumulationColor = vec4(finalColor, 1.0);
    ScreenColor = vec4(AccumulationColor.xyz / float(u_FrameIndex), 1.0);
}