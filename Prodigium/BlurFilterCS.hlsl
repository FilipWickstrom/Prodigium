#define MAXWEIGHTSIZE 24

RWTexture2D<float4> backBuffer : register(u0);

cbuffer BlurSettings : register(b5)
{
    uint blurRadius;
    bool useVertical;
    float2 padding;
    //Better to package it like this instead of separate floats
    float4 weights[MAXWEIGHTSIZE / 4];
};

//64 threads per group
[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float4 finalColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float2 offset;
    
    if (useVertical)
        offset = float2(1.0f, 0.0f);
    else
        offset = float2(0.0f, 1.0f);
    
    int start = blurRadius * -1;
    int end = blurRadius;
    uint counter = 0;
    
    for (int i = start; i <= end; i++)
    {
        //Unpackage the correct way from float4
        float weight = (weights[counter / 4])[counter % 4];
        
        finalColour += backBuffer[DTid.xy + (offset * i)] * weight;
        counter++;
    }
    
    backBuffer[DTid.xy] = finalColour;   
}