cbuffer LightsViewProj : register(b3)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer ModelMatrix : register(b1)
{
    float4x4 world;
};

float4 main(float3 position : POSITION) : SV_Position
{   
    float4x4 WVP = mul(mul(world, view), projection);
    
    float4 positionCS = mul(float4(position, 1.0f), WVP);

    return positionCS;
}