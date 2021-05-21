cbuffer camViewProj : register(b0)
{
    matrix view;
    matrix proj;
}

cbuffer Camera : register(b1)
{
    float4x4 viewMatrix;
    float4 camPos;
    float4 fogColour;
    float fogStart;
    float fogRange;
}

struct VertexOut
{
    float3 pos : POSITION;
};

struct GSOutput
{
    float4 pos : SV_Position;
};


[maxvertexcount(4)]
void main(point VertexOut input[1], uint primID : SV_PrimitiveID,
	inout TriangleStream<GSOutput> output)
{
    float3 position = input[0].pos;
    float3 toCamera = camPos.xyz - position.xyz;
    
    // Exclude particles that are too far away from the player.
    if (length(toCamera) > 300.0f)
        return;
    
    // Exclude particles too close to the camera.
    if (length(toCamera) < 15.0f)
        return;
    
    toCamera.y = 0.0f;
    toCamera = normalize(toCamera);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, toCamera);

    float width = 0.1f;
    float height = 0.3f;
    
    float4 positionW[4] =
    {
        float4(position + width * right - height * up, 1.0f),
		float4(position + width * right + height * up, 1.0f),
		float4(position - width * right - height * up, 1.0f),
		float4(position - width * right + height * up, 1.0f)
    };

    GSOutput geOut;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        geOut.pos = mul(mul(positionW[i], view), proj);
        output.Append(geOut);
    }
}