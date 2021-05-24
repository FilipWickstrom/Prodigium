#define MAXNROFBONES 25

//From current light
cbuffer Shadow : register(b3)
{
    float4x4 shadowView;
    float4x4 shadowProjection;
};

cbuffer BonesMatrices : register(b6)
{
    float4x4 bonesTransforms[MAXNROFBONES];
};

struct VertexShaderInput
{
    float3 position     : POSITION;
    uint4 boneIDs       : BONEIDS;
    float4 boneWeights  : BONEWEIGHTS;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
    float4x4 world = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        world += bonesTransforms[input.boneIDs[i]] * input.boneWeights[i];
    }
    
    float4x4 WVP = mul(mul(world, shadowView), shadowProjection);
   
    //Position in clipspace
    return mul(float4(input.position, 1.0f), WVP);
}