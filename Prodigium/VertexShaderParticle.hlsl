struct Vertex
{
    float3 position;
};

StructuredBuffer<Vertex> particles : register(t0);

struct VertexOut
{
    float3 position : POSITION;
};

VertexOut main( uint ID : SV_InstanceID)
{
    VertexOut output;
    
    output.position = particles[ID].position;
    
    return output;
}