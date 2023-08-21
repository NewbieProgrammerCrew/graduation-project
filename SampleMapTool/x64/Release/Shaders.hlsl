cbuffer cbPlayerInfo : register(b0)
{
    matrix gmtxPlayerWorld : packoffset(c0);
};
//카메라 객체의 데이터를 위한 상수 버퍼(스펙큘러 조명 계산을 위하여 카메라의 위치 벡터를 추가)
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};
//게임 객체의 데이터를 위한 상수 버퍼(게임 객체에 대한 재질 번호를 추가)
cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
    matrix gTexTransform : packoffset(c4);
    uint gnMaterial : packoffset(c8);
};

cbuffer cbGridInfo : register(b5)
{
    matrix gmtxGridObject : packoffset(c0);
};

 #include "Light.hlsl"

Texture2D gDiffuseMap : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);



SamplerState DefaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_DIFFUSED_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};
//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_DIFFUSED_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld),gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}
float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

VS_DIFFUSED_OUTPUT VSGrid(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGridObject), gmtxView), gmtxProjection);
    output.color = input.color;
    return (output);
}
float4 PSGrid(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

#define _WITH_VERTEX_LIGHTING
struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 TexC : TEXCOORD;
};
struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float2 TexC : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#else
    float3 normalW : NORMAL;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    float3 normalW = mul(input.normal, (float3x3) gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.color = Lighting(output.positionW, normalize(normalW));
#else
    output.normalW = normalW;
#endif
    //float4 texC = mul(float4(input.TexC, 0.0f, 1.0f), gTexTransform);
    output.TexC = mul(float4(input.TexC, 0.0f, 1.0f), gTexTransform);
    
    return (output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
    // Add a sampler to the Sample function.
    uint3 texCoord = uint3(0, 0, 0);
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, input.TexC);

#ifdef _WITH_VERTEX_LIGHTING
    return (input.color * diffuseAlbedo); 
#else
    //float3 normalW = normalize(input.normalW);
    //float4 color = Lighting(input.positionW, normalW);
    //float3 cal = float3(color.x, color.y, color.z) * normalW;
#endif
    return diffuseAlbedo;
}