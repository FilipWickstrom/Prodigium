struct Vertex
{
    float3 position;
};
RWStructuredBuffer<Vertex> buffer : register(u0);

cbuffer player : register(b0)
{
    float3 position;
}

[numthreads(8, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    // temp change to monsters position when he is in the game.
    // Distance from camera to monster.
    float3 dist = (position.xyz - float3(0.0f, 0.0f, 0.0f));
    
    // Factor that changes the speed of the rain depending on the distance to monster.
    float l = length(dist);
    if(l < 0)
        l *= -1;
    
    // Mutliplies the speed of the particle from a 1.0 to 0.1 scale.
    float factor = max(min(l, 200), 20.0f) * 0.5f;
    float speed = (factor) * 0.01f;
    
    // New position 
    float newPos = buffer[DTid.x].position.y - 2.0f * speed;
    
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