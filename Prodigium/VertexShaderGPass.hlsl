
cbuffer ViewProjBuffer : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
};

cbuffer WorldBuffer : register(b1)
{
    row_major float4x4 World;
};

struct VertexShaderInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal   : NORMAL;
};

struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
    float3 normal   : NORMAL;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    float4x4 WVP = mul(mul(World, View), Projection);
    
    output.position = mul(float4(input.position, 1.0f), WVP);
    output.normal = normalize(mul(input.normal, (float3x3) World));
    output.texCoord = input.texCoord;
      
	return output;
}