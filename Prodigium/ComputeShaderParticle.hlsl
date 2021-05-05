struct Vertex
{
    float3 position;
};
RWStructuredBuffer<Vertex> buffer;

[numthreads(8, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    buffer[DTid.x].position = float3(buffer[DTid.x].position.x, buffer[DTid.x].position.y - 0.001f, buffer[DTid.x].position.z);
}