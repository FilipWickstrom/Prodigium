struct PixelShaderInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct PixelShaderOutput
{
    float3 position : SV_Target0;
    float2 texCoord : SV_Target1;
    float3 normal : SV_Target2;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    output.position = input.position.xyz;
    output.texCoord = input.texCoord;
    output.normal = input.normal;
    
    return output;
}