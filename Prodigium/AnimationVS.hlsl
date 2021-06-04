#define MAXNROFBONES 25

cbuffer CameraViewProj : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer isNormalMapped : register(b2)
{
    float4 hasNormalMap;
}

cbuffer BonesMatrices : register(b6)
{
    float4x4 bonesTransforms[MAXNROFBONES];
};
    
struct VertexShaderInput
{
    float3 position     : POSITION;
    float2 texCoord     : TEXCOORD;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    uint4 boneIDs       : BONEIDS;
    float4 boneWeights  : BONEWEIGHTS;
    float4 specular     : SPECULAR;
};

struct VertexShaderOutput
{
    float4 positionCS : SV_Position;
    float4 positionWS : POSITIONWS;
    float2 texCoord   : TEXCOORD;
    float3 normalWS   : NORMAL;
    float4 tangent    : TANGENT;
    float4 viewPosNorm : SSAO;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    //Calculating the model/world-matrix for this vertex
    //depending on the bones weights. Each vertex can be 
    //affected by a maximum of 4 bones
    float4x4 world = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        world += bonesTransforms[input.boneIDs[i]] * input.boneWeights[i];
    }
    
    float4x4 WVP = mul(mul(world, view), projection);
    
    output.positionCS = mul(float4(input.position, 1.0f), WVP);
    
    output.positionWS = mul(float4(input.position, 1.0f), world);
    
    output.normalWS = normalize(mul(input.normal, (float3x3) world));
    
    output.texCoord = input.texCoord;
    
    // Last spot is for if it has normal map or not.
    output.tangent = float4(mul(input.tangent, (float3x3) world), hasNormalMap.x);
    
    
    float4 viewPos = mul(mul(float4(input.position, 1.0f), world), view);
    float3 viewNormal = normalize(mul(input.normal, (float3x3) mul(world, view)));
    output.viewPosNorm = float4(viewNormal.xyz, viewPos.z);
      
	return output;
}