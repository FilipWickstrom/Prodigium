#define MAXNROFBONES 40

cbuffer CameraViewProj : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer BonesMatrices : register(b2) //Dont wont to mess with others
{
    float4x4 bonesTransforms[MAXNROFBONES];
};
    
struct VertexShaderInput
{
    float3 position     : POSITION;
    float2 texCoord     : TEXCOORD;
    float3 normal       : NORMAL;
    //add tangent later for normalmap
    uint4 boneIDs       : BONEIDS;
    float4 boneWeights  : BONEWEIGHTS;
};

struct VertexShaderOutput
{
    float4 positionCS : SV_Position;
    float4 positionWS : POSITIONWS;
    float2 texCoord   : TEXCOORD;
    float3 normalWS   : NORMAL;
};


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    //Calculating the model/world-matrix for this vertex
    //depending on the bones weights. Each vertex can be 
    //affected by a maximum of 4  
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
      
	return output;
}