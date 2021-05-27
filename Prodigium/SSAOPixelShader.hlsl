Texture2D viewNormPosTexture : register(t0);

struct VertexOut
{
    float4 position : POSITION;
    float3 index : FARPLANEINDEX;
    float2 uv : UV;
};

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}