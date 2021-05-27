#define CORNERS 4
#define SAMPLES 14

cbuffer ssaoBuffer : register(b1)
{
    float4 frustumCorners[CORNERS];
    float4 offsetVectors[SAMPLES];
    matrix projection;
}

struct VertexIn
{
    float3 position : POSITION;
    float3 index : FARPLANEINDEX;
    float2 uv : UV;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 index : FARPLANEINDEX;
    float2 uv : UV;
};

VertexOut main( VertexIn input )
{
    VertexOut output;
    
    output.position = float4(input.position, 1.0f);
    output.index = frustumCorners[input.index.x].xyz;
    output.uv = input.uv;
	
    return output;
}