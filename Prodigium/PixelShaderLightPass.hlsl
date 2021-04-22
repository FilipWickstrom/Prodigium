
//Gbuffers
Texture2D G_positionWS   : register(t0);
Texture2D G_colour       : register(t1);
Texture2D G_normalWS     : register(t2);

SamplerState anisotropic : register(s0);

/*
Cbuffer with camera position?
*/

/*
Cbuffer with lights?
*/

//Easier to handle
struct GBuffers
{
    float4 positionWS;
    float4 colourTexture;
    float4 normalWS;
};

GBuffers GetGBuffers(float2 texCoords)
{
    GBuffers output;
    output.positionWS    = G_positionWS.Sample(anisotropic, texCoords);
    output.colourTexture = G_colour.Sample(anisotropic, texCoords);
    output.normalWS      = G_normalWS.Sample(anisotropic, texCoords);
    return output;
}


struct PixelShaderInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};



float4 main( PixelShaderInput input ) : SV_TARGET
{
    GBuffers gbuffers = GetGBuffers(input.texCoord);
    
    /*
    Do light calculations
    */
    
    
    
    //Returns the texture colour for now
    return gbuffers.colourTexture;   
}