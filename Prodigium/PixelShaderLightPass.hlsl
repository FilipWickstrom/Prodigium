//Gbuffers
Texture2D G_positionWS : register(t0);
Texture2D G_colour : register(t1);
Texture2D G_normalWS : register(t2);
SamplerState anisotropic : register(s0);

/*
Cbuffer with lights?
*/

struct lightBuffer
{
    float4 position; // positions.w is range.
    float4 direction; // direction.w is spot.
    float4 att; // att.w is type.
};

// Contains the information of how many lights are present.
// How many shadows should be rendered.
cbuffer LightsInfo : register(b0)
{
    float4 info;
}

#define TEMPCOUNT 9
static const float density = 0.007f;
static const float gradient = 0.2f;
cbuffer Camera : register(b1)
{
    float4x4 viewMatrix;
    float4 camPos;
    float4 fogColour;
    float fogStart;
    float fogRange;
}

struct LightViewProj
{
    matrix lightView;
    matrix lightProj;
};

StructuredBuffer<lightBuffer> lights : register(t3);
Texture2DArray shadowMaps : register(t4);
StructuredBuffer<LightViewProj> lightMatrices : register(t5);

//Easier to handle
struct GBuffers
{
    float4 positionWS;
    float4 diffuseColor;
    float4 normalWS;
};

GBuffers GetGBuffers(float2 texCoords)
{
    GBuffers output;
    output.positionWS = G_positionWS.Sample(anisotropic, texCoords);
    output.diffuseColor = G_colour.Sample(anisotropic, texCoords);
    output.normalWS = G_normalWS.Sample(anisotropic, texCoords);
    return output;
}


struct PixelShaderInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

float doShadow(float index, float4 lightViewPos)
{
    float shadowCoeff = 0.f;
    
    int shadowIndex = index - 1;

    // Homogenize coordinates to NDC
    lightViewPos.xy /= lightViewPos.w;
    
    // Transform positions from clip space [-1, 1] to texture space coords [0, 1] to sample shadow map, y is inverted to match UV y axis
    // ---- Example ::: (-1 / 2) = -0.5 ---> -0.5 + 0.5 = 0 ::: 1 / 2 = 0.5 ---> 0.5 + 0.5 = 1 :::
    float2 shadowCoord = float2(0.5f * lightViewPos.x + 0.5f, -0.5f * lightViewPos.y + 0.5f);
    
    float depth = lightViewPos.z / lightViewPos.w;

    float bias = 0.00003f;
    float dx = 1.0f / 4096.f;

    float s0 = shadowMaps.Sample(anisotropic, float3(shadowCoord, shadowIndex)).r + bias;
    float s1 = shadowMaps.Sample(anisotropic, float3(shadowCoord, shadowIndex) + float3(dx, 0.0f, 0.0f)).r + bias;
    float s2 = shadowMaps.Sample(anisotropic, float3(shadowCoord, shadowIndex) + float3(0.0f, dx, 0.0f)).r + bias;
    float s3 = shadowMaps.Sample(anisotropic, float3(shadowCoord, shadowIndex) + float3(dx, dx, 0.0f)).r + bias;
        
    float result0 = depth <= s0;
    float result1 = depth <= s1;
    float result2 = depth <= s2;
    float result3 = depth <= s3;
        
    float2 texelPos = shadowCoord * 4096.f;
    float2 lerps = frac(texelPos);
        
    shadowCoeff = lerp(lerp(result0, result1, lerps.x), lerp(result2, result3, lerps.x), lerps.y);
    
    return shadowCoeff;
}

float4 doSpotlight(float index, GBuffers buff, inout float4 s)
{
    float4 diff = float4(1.0f, 1.0f, 1.0f, 0.8f);
    float4 spec = float4(0.2f, 0.2f, 0.2f, 1.0f);
    float distance = length(lights[index].position.xyz - buff.positionWS.xyz);
    float3 lightVector = normalize(lights[index].position.xyz - buff.positionWS.xyz);
    
    int shadowIndex = index - 1;
    float4 lightViewPos = mul(buff.positionWS, lightMatrices[shadowIndex].lightView);
    lightViewPos = mul(lightViewPos, lightMatrices[shadowIndex].lightProj);
    
    float shadowCoeff = doShadow(index, lightViewPos);
    
    // Check if pixel is within the range of the spotlight

    float3 normals = normalize(buff.normalWS.xyz);
    float diffuseFactor = max(dot(normals, lightVector), 0.0f);
    diff *= diffuseFactor;
    
    if (diffuseFactor <= 0.f)
    {
        return diff * shadowCoeff;
    }

    float3 reflection = reflect(-lightVector, normals);
    // --change to camera pos--
    float3 toEye = normalize(camPos.xyz - buff.positionWS.xyz);
    spec *= pow(max(dot(reflection, toEye), 0.0f), 32.0f);

    float3 direction = normalize(lights[index].direction.xyz);

    // Nice effect to fade the lgiht at the rim of the cone
    float minCos = cos(15.f);
    float maxCos = (minCos + 1.0f) * 0.5f;
    float cosAngle = dot(direction, -lightVector);
    float spot = smoothstep(minCos, maxCos, cosAngle);
        
    float range = lights[index].position.w;
    float d = max(distance - range, 0);
        
    //Attenuate depending on distance from lightsource
    float denom = d / range + 1.f;
    float attenuation = 1.f / (denom * denom);
    float cutoff = 0.001f;
     
    // scale and bias attenuation such that:
    // attenuation == 0 at extent of max influence
    // attenuation == 1 when d == 0
    attenuation = (attenuation - cutoff) / (1 - cutoff) - 0.05f;
    attenuation = max(attenuation, 0);
        
    s += spec * attenuation * spot * shadowCoeff;
    diff *= attenuation * spot * shadowCoeff;

    
    return diff;
}

float4 doDirectional(float index, GBuffers buff, inout float4 s)
{
    float3 normals = normalize(buff.normalWS.xyz);
    float3 lightVec = -normalize(lights[index].direction.xyz);

    float4 diff = float4(0.1f, 0.1f, 0.2f, 0.35f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float diffuseFactor = max(dot(lightVec, normals), 0.0f);

    diff *= diffuseFactor;
    
    if (diffuseFactor <= 0.f)
    {
        return diff;
    }
    
    float3 v = reflect(-lightVec, normals);
    float3 toEye = normalize(camPos.xyz - buff.positionWS.xyz);
    float specFactor = pow(max(dot(v, toEye), 0.0f), 32.0f);

    s += spec * specFactor;

    return diff;
}

float4 doPointLight(float index, GBuffers buff, inout float4 s)
{
    float3 vecToLight = normalize(lights[index].position.xyz - buff.positionWS.xyz);
    float distance = length(lights[index].position.xyz - buff.positionWS.xyz);

    float3 normals = normalize(buff.normalWS.xyz);

    float4 diff = float4(1.0f, 1.0f, 1.0f, 0.8f);
    float4 spec = float4(0.07f, 0.07f, 0.07f, 1.0f);
    
    int shadowIndex = index - 1;
    float4 lightViewPos = mul(buff.positionWS, lightMatrices[shadowIndex].lightView);
    lightViewPos = mul(lightViewPos, lightMatrices[shadowIndex].lightProj);
    
    float shadowCoeff = doShadow(index, lightViewPos);
    
    //Diffuse light calculations
    float diffuseFactor = max(dot(vecToLight, normals), 0.0f);
    diff *= diffuseFactor;

    if(diffuseFactor <= 0.f)
    {
        return diff * shadowCoeff;
    }
    
    //Specular
    float3 toEye = normalize(camPos.xyz - buff.positionWS.xyz);
    float3 reflection = normalize(reflect(-vecToLight, normals));
    float specular = pow(max(dot(reflection, toEye), 0.0f), 32.0f);
        
    float range = lights[index].position.w;
    float d = max(distance - range, 0);
        
    //Attenuate depending on distance from lightsource
    float denom = d / range + 1.f;
    float attenuation = 1.f / (denom * denom);
    float cutoff = 0.01f;
     
    // scale and bias attenuation such that:
    // attenuation == 0 at extent of max influence
    // attenuation == 1 when d == 0
    attenuation = (attenuation - cutoff) / (1 - cutoff) - 0.1f;
    attenuation = max(attenuation, 0);
           
    // Add upp the specular
    s += spec * specular * attenuation * shadowCoeff;
            
    diff *= attenuation;
        
    return diff * shadowCoeff;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    GBuffers gbuffers = GetGBuffers(input.texCoord);
    float4 ambient = float4(0.04f, 0.04f, 0.04f, 0.02f) * gbuffers.diffuseColor;
    
    // This is for if no lights are present in the scene.
    if (info.x == 1)
    {
        return ambient;
    }
    
    /*
    Do light calculations
    */
    float4 lightColor = float4(0.0f, 0.0, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0, 0.0f, 0.0f);
    
    
    for (int i = 1; i < info.x; i++)
    {
        switch (lights[i].att.w)
        {
            case 0:
                lightColor += doDirectional(i, gbuffers, specular);
                break;
            case 1:
                lightColor += doPointLight(i, gbuffers, specular);
                break;
            case 2:
                lightColor += doSpotlight(i, gbuffers, specular);
                break;
            default:
                break;
        }
    }

    float4 finalColor = (saturate(lightColor) * gbuffers.diffuseColor + ambient) + saturate(specular);
        //FOG
        float3 toEye = camPos - gbuffers.positionWS;
        float distanceToEye = length(toEye);
    
        float fogFactor = saturate((distanceToEye - fogStart) / fogRange);
        finalColor = lerp(finalColor, fogColour, fogFactor);
    
    
  
        return finalColor;
}