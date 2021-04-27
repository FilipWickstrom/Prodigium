
//Camera location and where it's looking
cbuffer CameraViewProj : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

struct VertexOut
{
    float3 texCoord : SKYTEXCOORD;
    float4 posCS    : SV_Position;
};

VertexOut main(float3 skypos : SKYPOS)
{
    VertexOut output;
     
    //Coordinates to texture in 3d
    output.texCoord = skypos;
    
    float4x4 viewProj = mul(view, projection);
    
    //Sets the z = w. Later makes z/w = 1. Always as far as it can be in viewfrustum
    output.posCS = mul(float4(skypos, 0.0f), viewProj).xyww;
    
    return output;
}