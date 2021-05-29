#define MAXWEIGHTS 8

RWTexture2D<unorm float4> backBuffer : register(u0);

cbuffer BlurSettings : register(b5)
{
    uint blurRadius;
    bool useVertical;
    float2 padding;
    float4 weights[MAXWEIGHTS / 4];
};

//64 threads per group - best performance on all platforms
[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{  
    float4 finalColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weight = 0.0f;
    float2 offset = float2(0.0f, 0.0f);
    
    if (useVertical)
        offset = float2(1.0f, 0.0f);
    else
        offset = float2(0.0f, 1.0f);
        
    //Go through the full array
    for (uint pos = 0; pos <= blurRadius; pos++)
    {
        //Unpackage the correct way from float4
        weight = (weights[pos / 4])[pos % 4];
        finalColour += backBuffer[DTid.xy + (offset * pos)] * weight;
    }
    //Go back through the array and ignore the main weight
    for (uint neg = blurRadius; neg > 0; neg--)
    {
        weight = (weights[neg / 4])[neg % 4];
        finalColour += backBuffer[DTid.xy - (offset * neg)] * weight;
    }
    
    backBuffer[DTid.xy] = finalColour; 
}