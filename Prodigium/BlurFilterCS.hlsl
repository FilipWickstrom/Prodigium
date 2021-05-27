#define MAXWEIGHTS 16

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
    float width;
    float height;
    backBuffer.GetDimensions(width, height);
    
    float4 finalColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weight = 0.0f;
    float2 offset = float2(0.0f, 0.0f);
    
    if (useVertical)
        offset = float2(1.0f, 0.0f);        //width
    else
        offset = float2(0.0f, 1.0f);        //height     //Denna blir konstigt...
    
    //finalColour = backBuffer[DTid.xy];
    //finalColour += backBuffer[DTid.xy + offset];
    //finalColour += backBuffer[DTid.xy - offset];
    //finalColour /= 3;
    //backBuffer[DTid.xy] = finalColour;

    //Start to end of the array
    for (uint pos = 0; pos <= blurRadius; pos++)
    {
        //Unpackage the correct way from float4
        weight = (weights[pos / 4])[pos % 4];
        finalColour += backBuffer[DTid.xy + (offset * pos)] * weight;
    }
    
    //Ignore the first in the array which is highest
    for (uint neg = blurRadius; neg > 0; neg--)
    {
        weight = (weights[neg / 4])[neg % 4];
        finalColour += backBuffer[DTid.xy + (offset * -1 * neg)] * weight;
    }
    
    backBuffer[DTid.xy] = finalColour;
}