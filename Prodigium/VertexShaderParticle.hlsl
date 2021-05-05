struct VertexIn
{
    float4 position : POSITION;
};

struct VertexOut
{
    float4 position : SV_Position;
};

VertexOut main( VertexIn input)
{
    VertexOut output;
    
    output.position = input.position;
    
    return output;
}