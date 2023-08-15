###SHADER(VERTEX)
#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoords = aTexCoords;
}

###SHADER(FRAGMENT)
#version 430 core
#define FLT_MAX 3.402823466e+38F
#define UINT_MAX 4294967295.0
in vec2 TexCoords;

layout (location = 0) out vec4 AccumulationColor;
layout (location = 1) out vec4 ScreenColor;

uniform sampler2D u_AccumulationTexture;
uniform sampler2D u_ScreenTexture;

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
    float Roughness;
    float Metalic;
};

uniform int u_MaterialsCount;
layout(std430, binding = 1) buffer MaterialsBuffer
{
    Material b_Materials[];
};

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
    return float(seed) / UINT_MAX;
}

struct Ray
{
    vec3 Origin;
    vec3 Direction;
};

struct Payload
{
    vec4 Color;
    vec3 HitPosition;
    vec3 HitNormal;
    float HitDistance;
    int HitObject;
};

Payload Miss(in Ray ray)
{
    Payload payload;
    payload.Color = vec4(0.6, 0.7, 0.9, 1.0);
    payload.HitPosition = vec3(0);
    payload.HitNormal = vec3(0);
    payload.HitDistance = FLT_MAX;
    payload.HitObject = -1;
    
    return payload;
}

Payload ClosestHit(in Ray ray, in float closestDistance, in int closestObject)
{   
    Payload payload;
    payload.Color = vec4(b_Materials[b_Spheres[closestObject].MaterialId].Albedo, 1.0);
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
    
        Payload payload;
        payload.Color = vec4(0);
        payload.HitObject = -1;
        if (delta < 0.0)
        {
            continue;
        }
        
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

void main()
{
    const uint Frames = 1u;
    const uint MaxBounces = 5u;
    
    vec4 finalColor = vec4(0);
    
    for (uint frame = 1; frame <= Frames; frame++)
    {
        uint seed = uint(gl_FragCoord.y * u_Resolution.x + gl_FragCoord.x) * frame * u_FrameIndex;
        vec2 pixel = gl_FragCoord.xy / u_Resolution;
 
        Ray ray;
        ray.Origin = b_Camera.Position;
        vec4 coord = b_Camera.Projection * (2.0 * vec4(pixel, 1.0, 1.0) - 1.0);
        ray.Direction = vec3(b_Camera.View * vec4(normalize(coord.xyz / coord.w), 0));
    
        vec3 contribution = vec3(1);
    
        for (uint i = 0u; i < MaxBounces; i++)
        {
            seed += i;
        
            Payload payload = TraceRay(ray);
        
            if (payload.HitObject == -1)
            {
                finalColor += payload.Color * vec4(contribution, 1.0);
                break;
            }
        
            contribution *= b_Materials[b_Spheres[payload.HitObject].MaterialId].Albedo;
            
            ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001;
        
            vec3 unitSphere = 2.0 * vec3(FastRandom(seed), FastRandom(seed), FastRandom(seed)) - 1.0;
            ray.Direction = normalize(payload.HitNormal + unitSphere);
        }
    }
    
    finalColor = finalColor / float(Frames);
    
    if (u_FrameIndex != 1)
    {
        finalColor += texture(u_AccumulationTexture, TexCoords);
    }
        
    AccumulationColor = finalColor;
    ScreenColor = AccumulationColor / float(u_FrameIndex);
}