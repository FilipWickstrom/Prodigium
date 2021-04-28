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
    float4 diff = float4(0.8f, 0.8f, 0.8f, 0.8f);
    float4 spec = float4(0.1f, 0.1f, 0.1f, 0.0f);
    float4 amb = float4(0.3f, 0.3f, 0.3f, 0.3f);
    float3 lightVector = lights[index].position - buff.positionWS;
    float d = length(lightVector);
    
    if (d <= lights[index].position.w)
    {
        float3 normals = float3(buff.normalWS.x, buff.normalWS.y, buff.normalWS.z);
        lightVector /= d;
        float diffuse = dot(lightVector, normals);
        
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

        float spot = pow(max(dot(-lightVector, direction), 0.0f), 1.0f);

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

float4 doDirectional(float index)
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//float3 DirToLight = normalize(position.xyz - material.position.xyz); //Vektorn från objektet/materialet till ljuset

		//Diffuse light calculations
	//float NDotL = dot(DirToLight, material.normal); //Dot-produkten av objektets/materialets normal och vektorn från objektet/materialet till ljuset
	//float3 finalColor = DirLightColor.rgb * saturate(NDotL); //Lägger till diffuse light på ljusets färg

		//Specular light calculations
	//float3 toEye = EyePosition.xyz - position; //Vektor från objektet/materialet till kameran
	//toEye = normalize(toEye); //Normaliserar vektorn
	//float3 halfway = normalize(toEye + DirToLight); //Vet inte vad denna är till för
	//float NDotH = saturate(dot(halfway, material.normal)); //Dot-produkten av objektets/materialets normal och vektorn halfway
	//finalColor += DirLightColor.rgb * pow(NDotH, material.specExp) * material.specIntensity; //Lägger till specular light på ljusets färg

		//Final light calculations
	//finalColor * material.diffuseColor.rgb; //Lägger in ljusets färg på materialet

    return color;
}

float4 doPointLight(float index)
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//float3 lightDir = normalize(input.WorldPos - LightPosition);

		//Diffuse light calculations


		//Specular light calculations


    return color;
}

float4 main( PixelShaderInput input ) : SV_TARGET
{
    GBuffers gbuffers = GetGBuffers(input.texCoord);
    if (info.a == 1)
    {
        //Returns the texture colour for now
        return gbuffers.diffuseColor;
    }
    
    /*
    Do light calculations
    */
    float4 lightColor = float4(0.0f, 0.0, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0, 0.0f, 0.0f);
    float4 ambient = float4(0.5f, 0.5f, 0.5f, 0.5f);
    for (int i = 1; i < info.a; i++)
    {
        switch (lights[i].att.w)
        {
            case 0:
                lightColor += doDirectional(i);
                break;
            case 1:
                lightColor += doPointLight(i);
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
        lightColor = gbuffers.diffuseColor * ambient;
    }
    
    return saturate(lightColor) * gbuffers.diffuseColor + saturate(specular);
}