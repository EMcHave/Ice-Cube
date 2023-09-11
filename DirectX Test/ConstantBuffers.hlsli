cbuffer ConstantBufferNeverChanges : register(b0)
{
    float4 lightDirection;
    float4 lightPosition;
    float4 lightColor;
    float4 factors;
}

cbuffer ConstantBufferChangeOnResize : register(b1)
{
    matrix projection;
};

cbuffer ConstantBufferChangesEveryFrame : register(b2)
{
    matrix view;
};

cbuffer ConstantBufferChangesEveryPrim : register(b3)
{
    matrix world;
};