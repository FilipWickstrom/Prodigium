#define CORNERS 4
#define SAMPLES 14
#define radius .5f
#define start 1.f
#define end 2.0f
#define eps 0.5f

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
	// Sample to get normals xyz and depth w
    float4 tex = viewNormPosTexture.Sample(normSamp, input.uv);
    float3 normal = tex.xyz;
    normal = normalize(normal); // Re-normalize just in case
    float depth = tex.w;

	// Vector pointing towards far plane of viewed area, Interpolated between vertex points.
    float3 farPlaneVector = input.index;

	// Get position in view space. Reconstructed with the help of far plane vector
	// (w / z) * xyz
    float3 position = (depth / farPlaneVector.z) * farPlaneVector;

    float occlusion = 0.0f;

	// Random Vector to be sampled and reflected with SAMPLES unified vectors
    float3 randomVector = randomVecTexture.Sample(normSamp, input.uv).xyz;

	// Take samples around selected point
	[unroll] //unroll prepares shader to go through each iteration with no interruptions
    for (int i = 0; i < SAMPLES; i++)
    {
        float3 offset = reflect(offsetVectors[i].xyz, randomVector);

		// Sign returns -1 if below -1, 0 if 0, 1 if above 1. Flips vector if it is behind plane.
        float flip = sign(dot(offset, normal));

		// Offset point to sample from
        float3 offPoint = position + offset * radius * flip;

		// Texture equivalent, Clip space
        float4 texPoint = mul(float4(offPoint, 1.0f), projection);

		// Create projective coordinates to be sampled with
        float2 tex;
        tex.x = texPoint.x / texPoint.w / 2.0f + 0.5f;
        tex.y = -texPoint.y / texPoint.w / 2.0f + 0.5f;

		// Depth of texPoint
        float texDepth = viewNormPosTexture.Sample(normSamp, tex.xy).w;

		// Is point occluding our position? Reconstructed position
        float3 pot = (texDepth / offPoint.z) * offPoint;

		// Measure depth of position and potentional occluder
        float depthDist = position.z - pot.z;
		// How much occlusion does this occluder give?
        float d = max(dot(normal, normalize(pot - position)), 0.0f);

        float occ = 0.0f;
		// Calculate the occluder
        if (depthDist > eps)
        {
            occ = (end - depthDist) / (end - start);
        }
        
        float o = depthDist * d * occ;
		
        occlusion += o;
    }

    occlusion /= SAMPLES;

    float result = 1.0f - occlusion;

    return saturate(pow(result, 4.0f));
}