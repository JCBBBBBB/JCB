
struct VSInput
{
    float4 pos : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VS(VSInput input)
{
    VSOutput output;
    
    output.pos = input.pos;
    output.color = input.color;
    
    return output;
}


float4 PS(VSOutput input) : SV_TARGET
{
	return input.color;
}