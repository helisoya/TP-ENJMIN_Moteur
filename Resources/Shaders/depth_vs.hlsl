cbuffer LightData : register(b2)
{
    float4 LightPosition;
    float3 Direction;
    float4 Ambiant;
    float4 Diffuse;
    float4x4 LightView;
    float4x4 LightProjection;
    float pad;
};

cbuffer ModelData : register(b0)
{
    float4x4 Model;
};


struct Input
{
    float4 pos : POSITION0;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct Output
{
    float4 position : SV_POSITION;
};


Output main(Input input)
{
    Output output;
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.pos, Model);
    output.position = mul(output.position, LightView);
    output.position = mul(output.position, LightProjection);
	
	return output;
}