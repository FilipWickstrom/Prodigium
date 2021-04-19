Texture2D diffuseTexture : register(t0);
//Normalmap

SamplerState anisotropic : register(s0);

struct PixelShaderInput
{
    float4 position     : SV_Position;
    float2 texCoord     : TEXCOORD;
    float3 normal       : NORMAL;
};

struct PixelShaderOutput
{
    float4 position : SV_Target0;
    float4 uv  : SV_Target1;
    float4 normal   : SV_Target2;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    output.position = input.position;
    
    output.uv = diffuseTexture.Sample(anisotropic, input.texCoord);
    
    output.normal = float4(input.normal, 1.0f);
    
    return output;
}