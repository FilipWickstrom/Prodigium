cbuffer CameraViewProj : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer ModelMatrix : register(b1)
{
    float4x4 world;
};

cbuffer isNormalMapped : register(b2)
{
    float4 hasNormalMap;
}

struct VertexShaderInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float3 biTangent : BITANGENT;
};

struct VertexShaderOutput
{
    float4 positionCS : SV_Position;
    float4 positionWS : POSITIONWS;
    float2 texCoord   : TEXCOORD;
    float3 normalWS   : NORMAL;
    float4 tangent    : TANGENT;
    float3 biTangent  : BITANGENT;
    float4 viewPosNorm : SSAO;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    float4x4 WVP = mul(mul(world, view), projection);
    
    output.positionCS = mul(float4(input.position, 1.0f), WVP);
    
    output.positionWS = mul(float4(input.position, 1.0f), world);

    output.texCoord = input.texCoord;

    output.normalWS = normalize(mul(input.normal, (float3x3) world));
    
    // Last spot is for if it has normal map or not.
    output.tangent = float4(mul(input.tangent, (float3x3)world), hasNormalMap.x);
    
    float4 viewPos = mul(mul(float4(input.position, 1.0f), world), view);
    float3 viewNormal = normalize(mul(input.normal, (float3x3) mul(world, view)));
    output.viewPosNorm = float4(viewNormal.xyz, viewPos.z);

    
    output.biTangent = mul(input.biTangent, (float3x3)world);

	return output;
}