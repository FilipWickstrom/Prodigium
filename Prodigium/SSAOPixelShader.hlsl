#define CORNERS 4
#define SAMPLES 14
#define radius .5f
#define start 1.f
#define end 2.0f
#define eps .1f

Texture2D viewNormPosTexture : register(t0);
Texture2D randomVecTexture : register(t1);
SamplerState normSamp : register(s0);

cbuffer ssaoBuffer : register(b0)
{
    float4 frustumCorners[CORNERS];
    float4 offsetVectors[SAMPLES];
}

cbuffer viewProjection : register(b10)
{
    matrix view;
    matrix projection;
}

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 index : INDEX;
    float2 uv : UV;
};

float4 main(VertexOut input) : SV_TARGET
{
    float4 tex = viewNormPosTexture.Sample(normSamp, input.uv);
    float3 normal = tex.xyz;
    normal = normalize(normal);
    float depth = tex.w;
    float3 farPlaneVector = input.index;
    float3 position = (depth / farPlaneVector.z) * farPlaneVector;
    float occlusion = 0.0f;
    float3 randomVector = randomVecTexture.Sample(normSamp, input.uv).xyz;
	[unroll(SAMPLES)]
    for (int i = 0; i < SAMPLES; i++)
    {
        float3 offset = reflect(offsetVectors[i].xyz, randomVector);
        float flip = sign(dot(offset, normal));
        float3 offPoint = position + offset * radius * flip;
        float4 texPoint = mul(float4(offPoint, 1.0f), projection);
        float2 tex;
        tex.x = texPoint.x / texPoint.w / 2.0f + 0.5f;
        tex.y = -texPoint.y / texPoint.w / 2.0f + 0.5f;
        float texDepth = viewNormPosTexture.Sample(normSamp, tex.xy).w;
        float3 pot = (texDepth / offPoint.z) * offPoint;
        float depthDiff = position.z - pot.z;
        float d = max(dot(normal, normalize(pot - position)), 0.0f);

        float occ = 0.0f;
        if (depthDiff > eps)
            occ = (end - depthDiff) / (end - start);
        float o = depthDiff * d * occ;
		
        occlusion += o;
    }
    occlusion /= SAMPLES;
    float result = 1.0f - occlusion;
    return saturate(pow(result, 4.0f));
}