
RWTexture2D<unorm  float4> backBuffer : register(u0);

//
//struct 
//StructuredBuffer<floats> GuassFilter : register(t6);

cbuffer BlurSettings : register(b5)
{
    uint blurRadius;
    bool useVertical;
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
    //uint counter = 0;
    for (int i = start; i <= end; i++)
    {
        finalColour += backBuffer[DTid.xy + (offset * i)];  //* weight[counter++]
    }
    
    backBuffer[DTid.xy] = finalColour / (blurRadius*2 + 1); //remove blurRadius*2 + 1 then
        
    //Inverts colours for testing
    //backBuffer[DTid.xy] = float4(1.0f, 1.0f, 1.0f, 1.0f) - backBuffer[DTid.xy];
}