struct Input {
    float4 pos : POSITION0;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

cbuffer ModelData : register(b0)
{
    float4x4 Model;
};
cbuffer CameraData : register(b1)
{
    float4x4 View;
    float4x4 Projection;
};


struct Output {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    bool underWater : HEIGHT;
};


Output main(Input input) {
	Output output = (Output)0;
    
    output.pos = mul(input.pos, Model);
    
    output.underWater = input.pos.y < 19.5;
    
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    output.uv = input.uv;
    output.normal = mul(input.normal, Model);
    
	return output;
}