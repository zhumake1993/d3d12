#define MaxLights 16

struct Light
{
    float3 Strength;
    float FalloffStart; // 点光/聚光
    float3 Direction;   // 平行光/聚光
    float FalloffEnd;   // 点光/聚光
    float3 Position;    // 点光/聚光
    float SpotPower;    // 聚光
};

struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // 线性衰减
    return saturate((falloffEnd-d) / (falloffEnd - falloffStart));
}

// Fresnel反射的近似
// R0 = ( (n-1)/(n+1) )^2, n是折射率
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

    return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor*roughnessFactor;

	// specAlbedo可能会超出[0,1]的范围，超出LDR渲染的范围，需要规整
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

//---------------------------------------------------------------------------------------
// 计算平行光
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    // 光线向量与光的照射方向相反
    float3 lightVec = -L.Direction;

    // 兰伯特余弦定理
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// 计算点光
//---------------------------------------------------------------------------------------
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Position - pos;

    float d = length(lightVec);

    if(d > L.FalloffEnd)
        return 0.0f;

    lightVec /= d;

    // 兰伯特余弦定理
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // 线性衰减
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// 计算聚光
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Position - pos;

    float d = length(lightVec);

    if(d > L.FalloffEnd)
        return 0.0f;

    lightVec /= d;

    // 兰伯特余弦定理
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // 线性衰减
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    // 聚光衰变参数
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float4 ComputeLighting(Light gLights[MaxLights], Material mat,
                       float3 pos, float3 normal, float3 toEye,
                       float3 shadowFactor)
{
    float3 result = 0.0f;

    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
    }
#endif

#if (NUM_POINT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS+NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
    }
#endif 

    return float4(result, 0.0f);
}


