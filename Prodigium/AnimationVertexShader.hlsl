#define MAXNROFBONES 25

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
    float3 boneIDs      : BONEIDS;     //Make uint 
    float3 boneWeights  : BONEWEIGHTS;
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
    float4x4 viewproj = mul(view, projection);
    
    //Calculate the model-matrix for this vertex
    float4x4 bonesMatrices[3];
 
    for (int i = 0; i < 3; i++)
    {
        if ((uint)input.boneIDs[i] < MAXNROFBONES)
        {
            bonesMatrices[i] = bonesTransforms[(uint)input.boneIDs[i]] * input.boneWeights[i];   
        }
    }
    float4x4 world = bonesMatrices[0] * bonesMatrices[1] * bonesMatrices[2];
    
    float4x4 wvp = mul(viewproj, world);
    output.positionCS = mul(float4(input.position, 1.0f), wvp);
    
    output.positionWS = mul(float4(input.position, 1.0f), world);
    
    output.normalWS = mul(input.normal, (float3x3) world);
      
    output.texCoord = input.texCoord;
    
	return output;
}