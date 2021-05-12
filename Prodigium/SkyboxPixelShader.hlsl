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
    //TODO: Fix the skybox so it is the proper colour
    return lerp(float4(0.2f, 0.2f, 0.2f, 0.0f), cubemap.Sample(samplerState, input.texCoord), 0.2);
}