Texture2D tex : register(t0);
Texture2D shadowTex : register(t1);
SamplerState samplerState : register(s0);

cbuffer LightData : register(b0)
{
    float4 LightPosition;
    float3 Direction;
    float4 Ambiant;
    float4 Diffuse;
    float4x4 LightView;
    float4x4 LightProjection;
    float pad;
};

struct Input {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 posLight : LIGHT;
    bool underWater : HEIGHT;
};

float ShadowCalculation(float4 posLightSpace)
{
    // perform perspective divide
    float3 projCoords = posLightSpace.xyz / posLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = shadowTex.Sample(samplerState, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

float4 main(Input input) : SV_TARGET {

    // normalize normal
    input.normal = normalize(input.normal);

    // sample texture
    float4 color = tex.Sample(samplerState, input.uv / 16);
    
    float4 finalColor;

    // Apply ambiant color
    finalColor = color * Ambiant;
    
    // Apply Diffuse color
    finalColor += saturate(dot(float4(Direction, 0), input.normal) * Diffuse * color);
    
    // Underwater pass
    if (input.underWater)
    {
        finalColor *= float4(0.1, 0.1, 0.5, 1.0);
    }
    
    // calculate shadow
    //return input.posLight;
    float shadow = ShadowCalculation(input.posLight);
    //return float4(shadow, shadow, shadow, 1);
    //finalColor *= (1.0 - shadow);
    
    // Clipping
    finalColor.a = color.a;
    clip(finalColor.a - 0.01f);
    
    return finalColor;
    
}

