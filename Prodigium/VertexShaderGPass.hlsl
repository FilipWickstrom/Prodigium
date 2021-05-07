
cbuffer CameraViewProj : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer ModelMatrix : register(b1)
{
    float4x4 world;
};
cbuffer fogBuffer
{
    float fogStart;
    float fogEnd;
    
};
struct VertexShaderInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal   : NORMAL;
    //add tangent later for normalmap
};

struct VertexShaderOutput
{
    float4 positionCS : SV_Position;
    float4 positionWS : POSITIONWS;
    float2 texCoord   : TEXCOORD;
    float3 normalWS   : NORMAL;
    float fogFactor : FOG;
};
static const float density = 0.007f;
static const float gradient = 1.5f;
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    float4x4 WVP = mul(mul(world, view), projection);
    
    output.positionCS = mul(float4(input.position, 1.0f), WVP);
    
    output.positionWS = mul(float4(input.position, 1.0f), world);

    output.texCoord = input.texCoord;

    float4 posRelativeToCam = mul(output.positionWS, view);
    float distance = length(posRelativeToCam.xyz);
    output.normalWS = normalize(mul(input.normal, (float3x3) world));
    output.fogFactor = clamp(exp(-pow((distance * density), gradient)), 0.f,1.f);
	return output;
}