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


struct Output {
    float4 pos : SV_POSITION;
    float4 lightViewPosition : LIGHT0;
    float3 lightPos : LIGHT1;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    bool underWater : HEIGHT;
};


Output main(Input input) {
	Output output = (Output)0;

       
    float4 worldPosition;

    output.pos = mul(input.pos, Model);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPosition = mul(input.pos, Model);
    output.lightViewPosition = mul(output.lightViewPosition, LightView);
    output.lightViewPosition = mul(output.lightViewPosition, LightProjection);

    // Store the texture coordinates for the pixel shader.
    output.uv = input.uv;
    
    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, Model);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.pos, Model);

    // Determine the light position based on the position of the light and the position of the vertex in the world.
    output.lightPos = LightPosition.xyz - worldPosition.xyz;

    // Normalize the light position vector.
    output.lightPos = normalize(output.lightPos);

    return output;
}