Texture2D diffuseTexture : register(t0);
Texture2D normalTexture  : register(t1);
SamplerState anisotropic : register(s0);

struct PixelShaderInput
{
    float4 positionCS : SV_Position;
    float4 positionWS : POSITIONWS;
    float2 texCoord   : TEXCOORD;
    float3 normalWS   : NORMAL;
    float4 tangent : TANGENT;
    float4 viewPosNorm : SSAO;
};

struct PixelShaderOutput
{
    float4 positionWS : SV_Target0;
    float4 colour     : SV_Target1;
    float4 normalWS   : SV_Target2;
    float4 viewPosNorm : SV_Target3;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    output.positionWS = input.positionWS;
    
    output.colour = diffuseTexture.Sample(anisotropic, input.texCoord);
    
    float3 normalMap = normalTexture.Sample(anisotropic, input.texCoord).rgb;
    normalMap = normalMap * 2.0f - 1.0f;
    
    if(input.tangent.w >= 1.0f)
    {
        float3 tangent = input.tangent.xyz;
        tangent = normalize(tangent);
        float3 bitangent = cross(input.tangent.xyz, input.normalWS);
        bitangent = normalize(bitangent);
        float3x3 TBN = float3x3(tangent, bitangent, input.normalWS);
    
        output.normalWS = float4(mul(normalMap, TBN), 0.0f);
        output.normalWS = normalize(output.normalWS);
    }
    else
        output.normalWS = float4(input.normalWS, 0.0f);
    
    output.colour =  output.colour;
    output.viewPosNorm = input.viewPosNorm;
    return output;
}