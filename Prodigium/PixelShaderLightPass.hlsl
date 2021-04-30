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
cbuffer LightsInfo : register(b0)
{
    float4 info;
}

cbuffer Camera : register(b1)
{
    float4 camPos;
}

StructuredBuffer<lightBuffer> lights : register(t3);

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

float4 doSpotlight(float index, GBuffers buff, inout float4 s)
{
    float3 lightVector = lights[index].position.xyz - buff.positionWS.xyz;
    float d = length(lightVector);
    
    // Check if pixel is within the range of the spotlight

    float3 normals = normalize(buff.normalWS.xyz);
    lightVector /= d;
    float diffuse = max(dot(normals, lightVector), 0.0f);
        
    float4 diff = float4(0.8f, 0.8f, 0.8f, 0.8f);
    float4 spec = float4(0.2f, 0.2f, 0.2f, 1.0f);
    diff = diff * diffuse;

        [flatten]
    if (diffuse > 0.0f)
    {
        float3 reflection = reflect(-lightVector, normals);
            // --change to camera pos--
        float3 toEye = normalize(camPos.xyz - buff.positionWS.xyz);
        float specular = pow(max(dot(reflection, toEye), 0.0f), 32.0f);

        spec = spec * specular;
        float3 direction = normalize(lights[index].direction.xyz);

         // Nice effect to fade the lgiht at the rim of the cone
        float minCos = cos(lights[index].direction.w);
        float maxCos = (minCos + 1.0f) * 0.5f;
        float cosAngle = dot(direction, -lightVector);
        float spot = smoothstep(minCos, maxCos, cosAngle);
        
        float attenuation = 1 / (lights[index].att.x + (lights[index].att.y * d) + (lights[index].att.z * d * d));
        
        s += spec * attenuation * spot;
        diff *= attenuation * spot;
    }
    
    return diff;
}

float4 doDirectional(float index, GBuffers buff, inout float4 s)
{
    float3 normals = normalize(buff.normalWS.xyz);
    float3 lightVec = -normalize(lights[index].direction.xyz);

    float4 diff = float4(0.05f, 0.00f, 0.12f, 0.35f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float diffuseFactor = max(dot(lightVec, normals), 0.0f);

    diff *= diffuseFactor;
    
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

    //Distance check

    float3 normals = normalize(buff.normalWS.xyz);

    float4 diff = float4(1.0f, 1.0f, 1.0f, 0.8f);
    float4 spec = float4(0.07f, 0.07f, 0.07f, 1.0f);
        
        //Diffuse light calculations
    float diffuse = max(dot(vecToLight, normals), 0.0f);
    diff *= diffuse;
    [flatten]
    if (diffuse > 0.0f)
    {
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
        attenuation = (attenuation - cutoff) / (1 - cutoff);
        attenuation = max(attenuation, 0);
           
        // Add upp the specular
        s += spec * specular * attenuation;
            
        diff *= attenuation;
    }
        
    return diff;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    GBuffers gbuffers = GetGBuffers(input.texCoord);
    float4 ambient = float4(0.04f, 0.04f, 0.04f, 0.02f) * gbuffers.diffuseColor;
    
    // This is for if no lights are present in the scene.
    if (info.a == 1)
    {
        return ambient;
    }
    
    /*
    Do light calculations
    */
    float4 lightColor = float4(0.0f, 0.0, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0, 0.0f, 0.0f);
    
    for (int i = 1; i < info.a; i++)
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
    
    // If no lighting is reaching the pixel then apply default ambient lighting.
    if (lightColor.x <= 0)
    {
        return ambient;
    }
    
    return (saturate(lightColor) * gbuffers.diffuseColor + ambient) + saturate(specular);
}