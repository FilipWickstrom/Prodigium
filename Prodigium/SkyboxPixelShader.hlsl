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
    //return float4(1.0f, 0.0f, 0.0f, 0.0f);
	
    return cubemap.Sample(samplerState, input.texCoord);
}