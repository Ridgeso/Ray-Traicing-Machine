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
#define LOWETS_THRESHOLD 1.0e-6F
#define FLT_MAX 3.402823466e+38F
#define UINT_MAX 4294967295.0
#define PI 3.141592653589793

in vec2 TexCoords;

layout (location = 0) out vec4 AccumulationColor;
layout (location = 1) out vec4 ScreenColor;

uniform sampler2D AccumulationTexture;
uniform sampler2D RenderTexture;

uniform float DrawEnvironment;
uniform uint MaxBounces;
uniform uint MaxFrames;
uniform uint FrameIndex;
uniform vec2 Resolution;

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
    float RefractionRatio;
};

uniform int MaterialsCount;
layout(std430, binding = 1) buffer MaterialsBuffer
{
    Material b_Materials[];
};

struct
{
    uint seed;
} Global;

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

uniform int SpheresCount;
layout(std430, binding = 2) buffer SpheresBuffer
{
    Sphere b_Spheres[];
};

uint PCGhash(in uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float FastRandom(inout uint seed)
{
    seed = PCGhash(seed);
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

struct Pixel
{
    vec3 Color;
    vec3 Contribution;
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
    vec3 skyGradient = mix(SkyColorHorizon, SkyColorZenith, skyLerp);
    float sun = pow(max(0.0, dot(ray.Direction, -LightDir)), 500.0) * 100.0;
    vec3 skyColor = mix(GroundColor, skyGradient, groundToSky) + sun * float(groundToSky >= 1.0);
    
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
    
    for (int sphereId = 0; sphereId < SpheresCount; sphereId++)
    {
        vec3 origin = ray.Origin - b_Spheres[sphereId].Position;
        
        float a = dot(ray.Direction, ray.Direction);
        float b = 2.0 * dot(origin, ray.Direction);
        float c = dot(origin, origin) - pow(b_Spheres[sphereId].Radius, 2.0);
    
        float delta = b * b - 4.0 * a * c;
    
        if (delta < 0.0)
            continue;
        
        float sqrtDelta = sqrt(delta);
        
        float closest_t = (-b - sqrtDelta) / (2.0 * a);
        if (closest_t >= 0.0 && closest_t < closestDistance)
        {
            closestDistance = closest_t;
            closestObject = sphereId;
            continue;
        }
    }
    
    if (closestObject == -1)
        return Miss(ray);
    
    return ClosestHit(ray, closestDistance, closestObject);
}

void AccumulateColor(inout Pixel pixel, in int HitObject)
{
    pixel.Color += GetEmmision(b_Spheres[HitObject].MaterialId) * pixel.Contribution;
    pixel.Contribution *= b_Materials[b_Spheres[HitObject].MaterialId].Albedo;
}

bool Reflectance(in vec3 direction, in vec3 surfaceNormal, in float refIdx)
{
    float cosTheta = min(dot(-direction, surfaceNormal), 1.0);
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
 
    bool cannotRefract = refIdx * sinTheta > 1.0;
    
    float r0 = (1.0 - refIdx) / (1.0 + refIdx);
    r0 = r0 * r0;
    float r0p = r0 + (1.0 - r0) * pow(1 - cosTheta, 5);
    
    bool refractChance = r0p > FastRandom(Global.seed);
    
    return cannotRefract || refractChance;
}

void RefractRay(inout Ray ray, in Payload payload)
{
    float refractionRatio = dot(ray.Direction, payload.HitNormal) < 0.0 ?
        1.0 / b_Materials[b_Spheres[payload.HitObject].MaterialId].RefractionRatio :
        b_Materials[b_Spheres[payload.HitObject].MaterialId].RefractionRatio;
    
    if (Reflectance(ray.Direction, payload.HitNormal, refractionRatio))
    {
        ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001;
        ray.Direction = reflect(ray.Direction, payload.HitNormal);
    }
    else
    {
        ray.Origin = payload.HitPosition - payload.HitNormal * 0.0001;
        ray.Direction = refract(ray.Direction, payload.HitNormal, refractionRatio);
    }
}

void ReflectRay(inout Ray ray, in Payload payload)
{
    ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001;
    
    vec3 diffuseDir = normalize(payload.HitNormal + RandomUnitSpehere(Global.seed));
    vec3 specularDir = reflect(ray.Direction, payload.HitNormal);
    ray.Direction = mix(diffuseDir, specularDir, b_Materials[b_Spheres[payload.HitObject].MaterialId].Roughness);
}

void Scatter(inout Ray ray, in Payload payload)
{   
    if (b_Materials[b_Spheres[payload.HitObject].MaterialId].RefractionRatio > 1.0)
    {
        RefractRay(ray, payload);
    }
    else
    {
        ReflectRay(ray, payload);
    }        
}

void main()
{
    vec2 pixelCoord = gl_FragCoord.xy / Resolution;
    vec4 coord = b_Camera.Projection * (2.0 * vec4(pixelCoord, 1.0, 1.0) - 1.0);
    
    Ray precalculatedRay;
    precalculatedRay.Origin = b_Camera.Position;
    precalculatedRay.Direction = vec3(b_Camera.View * vec4(normalize(coord.xyz / coord.w), 0));
 
    Pixel pixel;
    pixel.Color = vec3(0);
    pixel.Contribution = vec3(1);
    
    for (uint frame = 1; frame <= MaxFrames; frame++)
    {
        Global.seed = uint(gl_FragCoord.y * Resolution.x + gl_FragCoord.x) + frame * FrameIndex * 735529;
        
        Ray ray = precalculatedRay;
        pixel.Contribution = vec3(1);
    
        for (uint i = 0u; i < MaxBounces; i++)
        {
            Global.seed += i;
        
            Payload payload = TraceRay(ray);
        
            if (payload.HitObject == -1)
            {
                pixel.Color += payload.Color * pixel.Contribution * DrawEnvironment;
                break;
            }
            
            AccumulateColor(pixel, payload.HitObject);
            
            Scatter(ray, payload);
        }
    }
    
    pixel.Color = pixel.Color / float(MaxFrames);
    
    if (FrameIndex != 1)
        pixel.Color += texture(AccumulationTexture, TexCoords).xyz;
        
    AccumulationColor = vec4(pixel.Color, 1.0);
    ScreenColor = vec4(AccumulationColor.xyz / float(FrameIndex), 1.0);
}