cbuffer camViewProj : register(b0)
{
    matrix view;
    matrix proj;
}

cbuffer camera : register(b1)
{
    float4 camPos;
}

cbuffer FixedValues
{
    float3 gravity = float3(0.0f, -9.8f, 0.0f);
    float2 uv_coord[4] =
    {
        float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
    };
};

struct VertexOut
{
    float3 pos : POSITION;
    float2 size : SIZE;
    uint id : IDENT;
};

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 size : SIZE;
};


[maxvertexcount(4)]
void main(point VertexOut input[1], uint primID : SV_PrimitiveID,
	inout TriangleStream<GSOutput> output)
{
	// Update age parameter
    float3 position = input[0].pos;

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 point_normal = camPos.xyz - position.xyz;
    point_normal.y = 0.0f;
    point_normal = normalize(point_normal);
    float3 right = cross(up, point_normal);

    float4 pos_w[4] =
    {
        float4(position + input[0].size.x * right - input[0].size.y * up, 1.0f),
		float4(position + input[0].size.x * right + input[0].size.y * up, 1.0f),
		float4(position - input[0].size.x * right - input[0].size.y * up, 1.0f),
		float4(position - input[0].size.x * right + input[0].size.y * up, 1.0f)
    };

    GSOutput geOut;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        geOut.pos = mul(mul(pos_w[i], view), proj);
        geOut.size = input[0].size;
        output.Append(geOut);
    }
}