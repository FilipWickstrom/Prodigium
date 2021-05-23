struct VertexOut
{
    float4 position : SV_Position;
};

float4 main() : SV_TARGET
{
	return float4(0.35f, 0.37f, 0.85f, 0.075f);
}