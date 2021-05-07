
Texture2D diffuseTexture : register(t0);
Texture2D normalTexture  : register(t1);
SamplerState anisotropic : register(s0);

struct PixelShaderInput
{
    float4 positionCS : SV_Position;
    float4 positionWS : POSITIONWS;
    float2 texCoord   : TEXCOORD;
    float3 normalWS   : NORMAL;
    float fogFactor   : FOG;
};

struct PixelShaderOutput
{
    float4 positionWS : SV_Target0;
    float4 colour     : SV_Target1;
    float4 normalWS   : SV_Target2;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    output.positionWS = input.positionWS;
    
    output.colour = diffuseTexture.Sample(anisotropic, input.texCoord);

    //LATER FIX: implement normalmap calculations here
    output.normalWS = float4(input.normalWS, 1.0f);
    output.colour = input.fogFactor * output.colour + (1.f - input.fogFactor) * float4(0.65f, 0.65f, 0.65f, 0.65f);
    return output;
}