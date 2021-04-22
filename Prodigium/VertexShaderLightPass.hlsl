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

    //Convert from [0, 1] coordinates to [-1, 1]
    output.position = float4((input.texCoord.x * 2) - 1, (1 - input.texCoord.y) * 2 - 1, 0.0f, 1.0f);

    output.texCoord = input.texCoord;
    
    return output;
}