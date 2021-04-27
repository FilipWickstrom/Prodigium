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

float4 doSpotlight(float index)
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return color;
}

float4 doDirectional(float index)
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//float3 DirToLight = normalize(position.xyz - material.position.xyz); //Vektorn fr�n objektet/materialet till ljuset

		//Diffuse light calculations
	//float NDotL = dot(DirToLight, material.normal); //Dot-produkten av objektets/materialets normal och vektorn fr�n objektet/materialet till ljuset
	//float3 finalColor = DirLightColor.rgb * saturate(NDotL); //L�gger till diffuse light p� ljusets f�rg

		//Specular light calculations
	//float3 toEye = EyePosition.xyz - position; //Vektor fr�n objektet/materialet till kameran
	//toEye = normalize(toEye); //Normaliserar vektorn
	//float3 halfway = normalize(toEye + DirToLight); //Vet inte vad denna �r till f�r
	//float NDotH = saturate(dot(halfway, material.normal)); //Dot-produkten av objektets/materialets normal och vektorn halfway
	//finalColor += DirLightColor.rgb * pow(NDotH, material.specExp) * material.specIntensity; //L�gger till specular light p� ljusets f�rg

		//Final light calculations
	//finalColor * material.diffuseColor.rgb; //L�gger in ljusets f�rg p� materialet

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
    if (info.a == 0)
    {
        //Returns the texture colour for now
        return gbuffers.diffuseColor;
    }
    
    /*
    Do light calculations
    */
    float4 lightColor = float4(0.0f, 0.0, 0.0f, 0.0f);
    for (int i = 0; i < info.a; i++)
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
                lightColor += doSpotlight(i);
                break;
        }
    }
    
    return saturate(lightColor) * gbuffers.diffuseColor;
}