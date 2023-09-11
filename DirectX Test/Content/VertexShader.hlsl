// A constant buffer that stores the three basic column-major matrices for composing geometry.
#include "ConstantBuffers.hlsli"

struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
    float4 pos = float4(input.pos, 1);
    
    pos = mul(pos, world);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    
    output.pos = pos;
    float4 normal = float4(input.normal, 0);
    normal = mul(normal, world);
    
    output.normal = normal.xyz;
    output.color = input.color;
    //output.worldPos = (float3) mul(input.pos, world);
	//float4 pos = float4(input.pos, 1.0f);

	return output;
}