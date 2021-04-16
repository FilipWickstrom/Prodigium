struct VertexShaderInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.position.w = 1.0f;
    output.position.xyz = input.position;
    output.texCoord = input.texCoord;
    
	return output;
}