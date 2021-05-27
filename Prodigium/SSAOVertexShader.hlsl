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
    output.index = input.index;
    output.uv = input.uv;
	
    return output;
}