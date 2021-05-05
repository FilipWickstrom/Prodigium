struct VertexOut
{
    float4 position : SV_Position;
};

float4 main() : SV_TARGET
{
	return float4(0.2f, 0.2f, 1.0f, 1.0f);
}