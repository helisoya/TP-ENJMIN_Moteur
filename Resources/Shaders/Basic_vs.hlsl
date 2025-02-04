struct Input {
	float3 pos : POSITION0;
    float4 color : COLOR0;
};

struct Output {
	float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

cbuffer MatrixData : register(b0)
{
    float4x4 Model;
    float4x4 View;
    float4x4 Projection;
}

Output main(Input input) {
	Output output = (Output)0;
	
    output.pos = float4(input.pos, 1);
    output.pos = mul(output.pos, Model);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    output.color = input.color;

	return output;
}