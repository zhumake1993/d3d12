#include "Common.hlsl"
 
struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// 获取材质数据
	MaterialData matData = gMaterialData[gMaterialIndex];
	
    // 变换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

	// 假定世界矩阵是正交的，否则需要计算逆转置矩阵
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);

    // 变换到其次剪裁空间
    vout.PosH = mul(posW, gViewProj);

	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// 获取材质数据
	MaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float  roughness = matData.Roughness;
	uint diffuseMapIndex = matData.DiffuseMapIndex;
	uint normalMapIndex = matData.NormalMapIndex;

	// 插值法向量会造成非单位法向量，因此需要规整
	pin.NormalW = normalize(pin.NormalW);

	float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	// Uncomment to turn off normal mapping.
	//bumpedNormalW = pin.NormalW;

	// 在数组中动态查找纹理
	diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// 环境光
    float4 ambient = gAmbientLight* diffuseAlbedo;

    const float shininess = (1.0f - roughness) * normalMapSample.a;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		bumpedNormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// 镜面反射
	float3 r = reflect(-toEyeW, bumpedNormalW);
	//r = BoxCubeMapLookup(pin.PosW, normalize(r), float3(0.0f, 0.0f, 0.0f), float3(2500.0f, 2500.0f, 2500.0f));
	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
	float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

    // 通常从漫反射材质中提取alpha值
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


