//--------------------------------------------------------------------------------------
struct vs_input_t
{
	float3 position : POSITION;
	float2 uv0 : UV0;
	float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
	float4 tint: TINT;
    float2 blendUV : BLEND_UV;
};

//--------------------------------------------------------------------------------------
struct v2f_t
{
	float4 position : SV_Position;
	float2 uv : UV;
	float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
	float4 tint : TINT;
	float3 worldPosition : WORLD_POSITION;
    float2 blendUV : BLEND_UV;
};

struct PointLight
{
    float4 lightColor;
    float4 lightPosition;
    float4 attenuation;
    float4 specularAttenuation;
};

cbuffer matrix_cb : register(b0)
{
	float4x4 MODEL;
	float4x4 VIEW;
	float4x4 PROJECTION;

    float4 EYE_POSITION;
};

cbuffer time_cb : register(b1)
{
	float TIME;

	float3 PADDING;
};

cbuffer light_cb : register(b2) 
{
	float4 AMBIENT;
	PointLight POINT_LIGHTS[8];
	
    float4 DIRECTIONAL_LIGHT_DIRECTION;
    float4 DIRECTIONAL_COLOR;

    float SPECULAR_POWER;
    float SPECULAR_FACTOR;
    float2 UNUSED;
};

Texture2D<float4> tBlendTexture : register(t0);
Texture2D<float4> tDiffuse1 : register(t1);
Texture2D<float4> tDiffuse2 : register(t2);
Texture2D<float4> tDiffuse3 : register(t3);
Texture2D<float4> tDiffuse4 : register(t4);
Texture2D<float4> tNormal1 : register(t5);
Texture2D<float4> tNormal2 : register(t6);
Texture2D<float4> tNormal3 : register(t7);
Texture2D<float4> tNormal4 : register(t8);
Texture2D<float4> tSpecular1 : register(t9);
Texture2D<float4> tSpecular2 : register(t10);
Texture2D<float4> tSpecular3 : register(t11);
Texture2D<float4> tSpecular4 : register(t12);
SamplerState sSampler : register(s0);


float RangeMap(float d0, float d1, float r0, float r1, float value)
{
	return ((value - d0) / (d1 - d0)) * (r1 - r0) + r0;
}


float4 FourWayBlend(float4 color1, float4 color2, float4 color3, float4 color4, float weight1, float weight2, float weight3, float weight4)
{
    float4 blendedColor = (color1 * weight1) + (color2 * weight2) + (color3 * weight3) + (color4 * weight4);
    return blendedColor;
}

float3 FourWayBlend(float3 color1, float3 color2, float3 color3, float3 color4, float weight1, float weight2, float weight3, float weight4)
{
    float3 blendedColor = (color1 * weight1) + (color2 * weight2) + (color3 * weight3) + (color4 * weight4);
    return blendedColor;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
v2f_t VertexFunction(vs_input_t input)
{
	v2f_t v2f = (v2f_t)0;

	float4 model = float4(input.position.xyz, 1.f);
	float4 world = mul(model, MODEL);
	float4 view = mul(world, VIEW);
	float4 finalPosition = mul(view, PROJECTION);

	v2f.position = finalPosition;
	v2f.normal = mul(float4(input.normal, 0.f), MODEL).xyz;
    v2f.tangent = mul(float4(input.tangent, 0.f), MODEL).xyz;
    v2f.bitangent = mul(float4(input.bitangent, 0.f), MODEL).xyz;
	v2f.uv = input.uv0;
	v2f.tint = input.tint;
	v2f.worldPosition = world.xyz;
    v2f.blendUV = input.blendUV;
	return v2f;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 FragmentFunction(v2f_t input) : SV_Target0 // semeantic of what I'm returning
{
    float height = input.worldPosition.y;
    float heightBelowSeaLevel = -height;

    float4 blendWeights;
    blendWeights.r = abs(7.f - height);
    if(blendWeights.r < 0.f)
        blendWeights.r = 0.f;

    blendWeights.g = abs(4.f - height);
    if (blendWeights.g < 0.f)
        blendWeights.g = 0.f;

    blendWeights.b = abs(2.f - heightBelowSeaLevel);
    if (blendWeights.b < 0.f)
        blendWeights.b = 0.f;

    blendWeights.a = abs(5.f - heightBelowSeaLevel);
    if (blendWeights.a < 0.f)
        blendWeights.a = 0.f;
    blendWeights = normalize(blendWeights);
    blendWeights = float4(1.f, 1.f, 1.f, 1.f) - blendWeights;

	float4 blendColor = tBlendTexture.Sample(sSampler, input.blendUV);
    //blendColor = normalize(blendColor);
    blendColor = blendColor * blendColor * blendColor;
    blendColor = normalize(blendColor);
    blendWeights += (blendColor * 0.25);
    blendWeights = blendWeights * blendWeights;
    blendWeights = normalize(blendWeights);

    float4 color1 = tDiffuse1.Sample(sSampler, input.uv);
    float4 color2 = tDiffuse2.Sample(sSampler, input.uv);
    float4 color3 = tDiffuse3.Sample(sSampler, input.uv);
    float4 color4 = tDiffuse4.Sample(sSampler, input.uv);

    float3 normalColor1 = tNormal1.Sample(sSampler, input.uv).xyz;
    float3 normalColor2 = tNormal2.Sample(sSampler, input.uv).xyz;
    float3 normalColor3 = tNormal3.Sample(sSampler, input.uv).xyz;
    float3 normalColor4 = tNormal4.Sample(sSampler, input.uv).xyz;

    float3 specularMapColor1 = tSpecular1.Sample(sSampler, input.uv).xyz;
    float3 specularMapColor2 = tSpecular2.Sample(sSampler, input.uv).xyz;
    float3 specularMapColor3 = tSpecular3.Sample(sSampler, input.uv).xyz;
    float3 specularMapColor4 = tSpecular4.Sample(sSampler, input.uv).xyz;

    float4 color = FourWayBlend(color1, color2, color3, color4, blendWeights.r, blendWeights.g, blendWeights.b, blendWeights.a);
    float3 normalColor = FourWayBlend(normalColor1, normalColor2, normalColor3, normalColor4, blendWeights.r, blendWeights.g, blendWeights.b, blendWeights.a);
    float3 specularMapColor = FourWayBlend(specularMapColor1, specularMapColor2, specularMapColor3, specularMapColor4, blendWeights.r, blendWeights.g, blendWeights.b, blendWeights.a);

    float3 surfaceNormal = normalColor * float3(2.f, 2.f, 1.f);
    surfaceNormal += float3(-1.f, -1.f, 0.f);
    surfaceNormal = normalize(surfaceNormal);

	float3 normal = normalize(input.normal);
    float3 tangent = input.tangent;
    float3 bitangent = normalize(cross(tangent, normal));
    tangent = normalize(cross(normal, bitangent));

    float3x3 tbn = { tangent, bitangent, normal };

    surfaceNormal = mul(surfaceNormal, tbn);

	float4 diffuse = color * input.tint;


    float3 vectorToEye = EYE_POSITION.xyz - input.worldPosition;
    float3 eyeVector = -normalize(vectorToEye); // get direction from eye to fragment

	float4 ambientFactor = float4(AMBIENT.xyz * AMBIENT.w, 1.f);

	float4 lightFactor = float4(0.f, 0.f, 0.f, 0.f);
    float4 specularFactor = float4(0.f, 0.f, 0.f, 0.f);

	for (int pointLightIndex = 0; pointLightIndex < 8; pointLightIndex++)
	{
		float4 lightColor = POINT_LIGHTS[pointLightIndex].lightColor;
		float4 lightPosition = POINT_LIGHTS[pointLightIndex].lightPosition;
		float4 attenuation = POINT_LIGHTS[pointLightIndex].attenuation;
        float4 specularAttenuation = POINT_LIGHTS[pointLightIndex].specularAttenuation;

		float3 vectorToLight = lightPosition.xyz - input.worldPosition;
		float distanceToLight = length(vectorToLight);
		float3 dirToLight = vectorToLight / distanceToLight;

		float dot3 = saturate(dot(dirToLight, surfaceNormal));
		float attenuationFactor = lightColor.w / (attenuation.x
		+ distanceToLight * attenuation.y
		+ distanceToLight * distanceToLight * attenuation.z);

		float4 finalLightColor = float4(lightColor.xyz, 1.0f);
		lightFactor += finalLightColor * dot3 * attenuationFactor;

        // Calculate Spec Component
        float specAttenuation = lightColor.w / (specularAttenuation.x 
        + distanceToLight * specularAttenuation.y
        + distanceToLight * distanceToLight * specularAttenuation.z);
   
        // figure how much the reflected vector coincides with the eye vector
        float3 refLightDir = reflect(dirToLight, surfaceNormal);
        float specDot3 = saturate(dot(refLightDir, eyeVector));

        // take it to the spec power, and scale it by the spec factor and our attenuation
        float specFactor = specAttenuation * SPECULAR_FACTOR * pow(specDot3, SPECULAR_POWER);
        float4 specColor = specFactor * lightColor;
        specularFactor += (specColor * float4(specularMapColor, 1.f));
    }

    //Directional light
    float3 dirToLight = -DIRECTIONAL_LIGHT_DIRECTION.xyz;

    float dot3 = saturate(dot(dirToLight, surfaceNormal));
    float4 finalLightColor = float4(DIRECTIONAL_COLOR.xyz, 1.0f);
    lightFactor += ((finalLightColor * dot3) * DIRECTIONAL_COLOR.w);

    float3 refLightDir = reflect(dirToLight, surfaceNormal);
    float specDot3 = saturate(dot(refLightDir, eyeVector));
    float specFactor = SPECULAR_FACTOR * pow(specDot3, SPECULAR_POWER);
    float4 specColor = specFactor * DIRECTIONAL_COLOR;
    specularFactor += ((specColor * float4(specularMapColor, 1.f)) * DIRECTIONAL_COLOR.w);

	float4 diffuseFactor = saturate(ambientFactor + lightFactor);
	return (diffuse * diffuseFactor) + specularFactor;
}