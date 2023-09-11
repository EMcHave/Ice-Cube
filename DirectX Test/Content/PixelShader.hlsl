#include "ConstantBuffers.hlsli"

struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

float4 main(VertexShaderOutput input) : SV_Target
{   
    float3 dir = normalize(lightDirection.xyz);
    float3 norm = normalize(input.normal);
    
    float4 ambient = (lightColor * ambient_factor) * input.color;
    float4 diffuse = saturate(dot(dir, norm) * lightColor);
    
    float4 out_color = (ambient + diffuse) * input.color;
    out_color.a = 1;
    return out_color;   
}