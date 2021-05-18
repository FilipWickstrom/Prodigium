struct Vertex
{
    float3 position;
};
RWStructuredBuffer<Vertex> buffer : register(u0);

cbuffer buffer : register(b0)
{
    float4 speed;
}

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // New position 
    float newPos = buffer[DTid.x].position.y - 2.5f * speed.x;
    
    // Current Position
    float currentPos = buffer[DTid.x].position.y;
    
    // Check if particle is below ground.
    if(currentPos >= -10.0f)
    {
        buffer[DTid.x].position.y = newPos;
    }
    // Respawn particle
    else
        buffer[DTid.x].position.y += 235.0f;
}