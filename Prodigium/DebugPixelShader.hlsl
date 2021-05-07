struct PixelShaderInput
{
    float4 position : SV_Position;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    color.xyz = input.position.xyz;
    color.xyz = color.xyz * float3(0.0f, 1.0f, 0.0f);
	
    return color;
}