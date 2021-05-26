SamplerState samp : register(s0);
Texture2D albedoTex : register(t0);
Texture2D opacityTex : register(t1);

struct VertexOut
{
    float4 position : SV_Position;
    float2 uv : UV;
};

float4 main(VertexOut input) : SV_TARGET
{
    float3 color = albedoTex.Sample(samp, input.uv).rgb;
    float opacity = opacityTex.Sample(samp, input.uv).r;
	
    return float4(color, (opacity * 2.5));
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
}