Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

cbuffer LightData : register(b0)
{
    float3 Direction;
    float4 Ambiant;
    float4 Diffuse;
    float pad;
};

struct Input {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET {

    
    input.normal = normalize(input.normal);
    //input.normal = input.normal / 2 + 0.5f;

    float4 color = tex.Sample(samplerState, input.uv / 16);
    
    //return color;
    //return input.normal;
    
    float4 finalColor;

    finalColor = color * Ambiant;
    finalColor += saturate(dot(float4(Direction, 0), input.normal) * Diffuse * color);
    
    finalColor.a = color.a;
    clip(finalColor.a - 0.01f);
    
    return finalColor;
}