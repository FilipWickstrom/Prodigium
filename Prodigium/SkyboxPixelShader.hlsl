//Cubemap texture
TextureCube cubemap : register(t0);

//Special sampler for the cube
SamplerState samplerState : register(s0);

struct InputFromVertex
{
    float3 texCoord : SKYTEXCOORD;
    float4 posCS    : SV_Position;
};

float4 main(InputFromVertex input) : SV_TARGET
{
    return cubemap.Sample(samplerState, input.texCoord) * float4(0.2f,0.2f,0.2f,1.0f);
}