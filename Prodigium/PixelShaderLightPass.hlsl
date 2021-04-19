
Texture2D GBuff_Position    : register(t0);
Texture2D GBuff_Texture     : register(t1);
Texture2D GBuff_Normal      : register(t2);
//Material?

SamplerState anisotropic    : register(s0);

struct PixelShaderInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 finalColour = {0,0,0,0};
    
    //Sample the texture - TESTING
    //finalColour = GBuff_Texture.Sample(anisotropic, input.texCoord);
    
    return finalColour;
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}