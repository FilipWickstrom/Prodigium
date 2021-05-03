cbuffer CameraViewProj : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer ModelMatrix : register(b1)
{
    float4x4 world;
};

struct VertexShaderInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexShaderOutput
{
    float4 positionCS;
};

VertexShaderOutput main(VertexShaderInput input) : SV_Position
{
    VertexShaderOutput output;
    
    float4x4 WVP = mul(mul(world, view), projection);
    
    output.positionCS = mul(float4(input.position, 1.0f), WVP);

    return output;
}