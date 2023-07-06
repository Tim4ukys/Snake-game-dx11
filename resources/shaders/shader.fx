cbuffer ConstantBuffer : register(b0)
{
    matrix constScreenScale;
    matrix constPos;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output;
    output.Pos = mul(Pos, constPos);
    output.Pos = mul(output.Pos, constScreenScale);
    output.Color = Color;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return input.Color;
}