struct Input {
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

float4 main(Input input) : SV_TARGET {
    return input.color;

}