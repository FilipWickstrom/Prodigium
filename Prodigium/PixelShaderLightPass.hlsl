//Gbuffers
Texture2D G_positionWS   : register(t0);
Texture2D G_colour       : register(t1);
Texture2D G_normalWS     : register(t2);

SamplerState anisotropic : register(s0);

/*
Cbuffer with camera position?
*/

/*
Cbuffer with lights?
*/

struct lightBuffer
{
    float4 position; // positions.w is range.
    float4 direction; // direction.w is spot.
    float4 att; // att.w is type.
};

// Contains the information of how many lights are present.
cbuffer LightsInfo : register(b0)
{
    float4 info;
}

StructuredBuffer<lightBuffer> lights : register(t3);

//Easier to handle
struct GBuffers
{
    float4 positionWS;
    float4 diffuseColor;
    float4 normalWS;
};

GBuffers GetGBuffers(float2 texCoords)
{
    GBuffers output;
    output.positionWS    = G_positionWS.Sample(anisotropic, texCoords);
    output.diffuseColor = G_colour.Sample(anisotropic, texCoords);
    output.normalWS      = G_normalWS.Sample(anisotropic, texCoords);
    return output;
}


struct PixelShaderInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

float4 doSpotlight(float index, GBuffers buff, inout float4 s)
{
    float3 lightVector = lights[index].position - buff.positionWS;
    float d = length(lightVector);
    
    if (d <= lights[index].position.w)
    {
        float3 normals = float3(buff.normalWS.x, buff.normalWS.y, buff.normalWS.z);
        lightVector /= d;
        float diffuse = dot(lightVector, normals);
        
        float4 diff = float4(0.8f, 0.8f, 0.8f, 0.8f);
        float4 spec = float4(0.1f, 0.1f, 0.1f, 0.0f);
        float4 amb = float4(0.3f, 0.3f, 0.3f, 0.3f);
        [flatten]
        if (diffuse > 0.0f)
        {
            float3 reflection = reflect(-lightVector, normals);
            // --change to camera pos--
            float3 toEye = float4(0, 0, 0, 0) - buff.positionWS;
            toEye = normalize(toEye);
            float specular = pow(max(dot(reflection, toEye), 0.0f), 32.0f);

            diff = diff * diffuse;
            spec = spec * specular;
        }
        
        float3 direction = float3(lights[index].direction.x, lights[index].direction.y, lights[index].direction.z);

        float spot = pow(max(dot(-lightVector, direction), 0.0f), lights[index].direction.w);

        float att = spot / dot(float3(lights[index].att.x, lights[index].att.y, lights[index].att.z), float3(1.0f,
        d, d * d));

        s += spec;
        return (amb + diff);
    }
    else
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

float4 doDirectional(float index, GBuffers buff, inout float4 s)
{
    float3 normals = float3(buff.normalWS.x, buff.normalWS.y, buff.normalWS.z);

    float4 diff = float4(0.8f, 0.8f, 0.8f, 0.8f);
    float4 spec = float4(0.1f, 0.1f, 0.1f, 0.0f);
    float4 amb = float4(0.3f, 0.3f, 0.3f, 0.3f);

	float3 DirToLight = normalize(lights[index].position.xyz - buff.positionWS.xyz); 

		//Diffuse light calculations
	float NDotL = dot(DirToLight, normals); 
    float4 diffuse = diff * NDotL;


		//Specular light calculations
    float4 toEye = normalize(float4(0, 0, 0, 0) - buff.positionWS); //camera pos behövs här
	float4 halfway = normalize(toEye + float4(DirToLight, 0)); //Vet inte vad denna är till för
	float NDotH = saturate(dot(halfway, normals)); 
    float4 specular = spec * pow(max(NDotH, 0), 32.0f);
    
    s += specular;
		
    return (amb + diffuse);
}

float4 doPointLight(float index, GBuffers buff, inout float4 s)
{
    
	float3 vecToLight = lights[index].position.xyz - buff.positionWS.xyz;
    float distance = length(vecToLight);
    vecToLight = normalize(vecToLight);

        //Distance check
   if(distance <= lights[index].position.w)
    {
       float3 normals = float3(buff.normalWS.x, buff.normalWS.y, buff.normalWS.z);

       float4 diff = float4(0.5f, 0.5f, 0.5f, 0.8f);
       float4 spec = float4(0.1f, 0.1f, 0.1f, 1.0f);
       float4 amb = float4(0.3f, 0.3f, 0.3f, 0.3f);
       
            //Diffuse light calculations
       float diffuse = dot(vecToLight, normals);
       

       if (diffuse > 0.0f)
       {
           //Specular light calculations

           float4 toEye = normalize(float4(0, 0, 0, 0) - buff.positionWS); //camera pos behövs här
           float4 reflection = float4(normalize(2 * dot(normals, vecToLight) * normals - vecToLight), 1);

           float4 specular = spec * pow(max(dot(reflection, toEye), 0), 32.0f);
           
           diff *= diffuse;
           spec *= specular;
       }

       s += spec;

       return (amb + diff + spec) /*/ (lights[index].att.x + (lights[index].att.y * distance) + (lights[index].att.z * (distance * distance)))*/;

    }

   else
       return float4(0.0f, 0.0f, 0.0f, 0.0f);
		
}

float4 main( PixelShaderInput input ) : SV_TARGET
{
    GBuffers gbuffers = GetGBuffers(input.texCoord);
    float4 ambient = float4(0.25f, 0.25f, 0.25f, 0.25f);
    
    // This is for if no lights are present in the scene.
    if (info.a == 1)
    {
        return gbuffers.diffuseColor * ambient;
    }
    
    /*
    Do light calculations
    */
    float4 lightColor = float4(0.0f, 0.0, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0, 0.0f, 0.0f);
    
    for (int i = 1; i < info.a; i++)
    {
        switch (lights[i].att.w)
        {
            case 0:
                lightColor += doDirectional(i, gbuffers, specular);
                break;
            case 1:
                lightColor += doPointLight(i, gbuffers, specular);
                break;
            case 2:
                lightColor += doSpotlight(i, gbuffers, specular);
                break;
            default:
                break;
        }
    }
    
    // If no lighting is reaching the pixel then apply default ambient lighting.
    if (lightColor.x <= 0)
    {
        return gbuffers.diffuseColor * ambient;
    }
    
    return saturate(lightColor) * gbuffers.diffuseColor + saturate(specular);
}