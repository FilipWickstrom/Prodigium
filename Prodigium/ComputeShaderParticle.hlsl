struct Vertex
{
    float3 position;
};
RWStructuredBuffer<Vertex> buffer : register(u0);

[numthreads(8, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    buffer[DTid.x].position.y -= 0.001;
}