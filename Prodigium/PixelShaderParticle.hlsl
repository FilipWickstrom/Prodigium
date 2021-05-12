struct VertexOut
{
    float4 position : SV_Position;
};

float4 main() : SV_TARGET
{
	return float4(0.075f, 0.075f, 0.7f, 0.075f);
}