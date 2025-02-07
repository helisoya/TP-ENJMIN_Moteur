Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET {
    float4 color = tex.Sample(samplerState, input.uv / 2);
    return color;
}