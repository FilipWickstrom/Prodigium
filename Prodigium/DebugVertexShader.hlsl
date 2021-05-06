cbuffer CameraViewProj : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

struct VertexShaderInput
{
    float3 position : POSITION;
};

struct VertexShaderOutput
{
    float4 position : SV_Position;
};

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
	
    output.position.xyz = input.position;
    output.position.w = 1.0f;
    
    float4x4 VP = mul(view, projection);
    
    output.position = mul(output.position, VP);
	
	return output;
}