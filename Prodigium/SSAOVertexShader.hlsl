struct VertexIn
{
    float3 position : POSITION;
    float3 index : FARPLANEINDEX;
    float2 uv : UV;
};

struct VertexOut
{
    float3 position : POSITION;
    float3 index : FARPLANEINDEX;
    float2 uv : UV;
};

VertexOut main( VertexIn input )
{
    VertexOut output;
	
    output = input;
	
    return output;
}