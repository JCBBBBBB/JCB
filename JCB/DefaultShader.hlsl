
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct VSInput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput VS(VSInput input)
{
    VSOutput output;
    
    output.pos = input.pos;
    output.uv = input.uv;
    
    return output;
}


float4 PS(VSOutput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
}