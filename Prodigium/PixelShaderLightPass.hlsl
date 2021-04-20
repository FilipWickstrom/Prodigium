Texture2D catTexture : register(s0);

struct PixelShaderInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

float4 main() : SV_TARGET
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}