cbuffer cbSsao : register(b0)
{
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gProjTex;
	float4   gOffsetVectors[14];

    // 用于SsaoBlur.hlsl
    float4 gBlurWeights[3];

    float2 gInvRenderTargetSize;

    // 遮蔽参数
    float    gOcclusionRadius;
    float    gOcclusionFadeStart;
    float    gOcclusionFadeEnd;
    float    gSurfaceEpsilon;
};

cbuffer cbRootConstants : register(b1)
{
    bool gHorizontalBlur;
};
 
Texture2D gNormalMap    : register(t0);
Texture2D gDepthMap     : register(t1);
Texture2D gRandomVecMap : register(t2);

SamplerState gsamPointClamp : register(s0);
SamplerState gsamLinearClamp : register(s1);
SamplerState gsamDepthMap : register(s2);
SamplerState gsamLinearWrap : register(s3);

static const int gSampleCount = 14;
 
static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};
 
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosV : POSITION;
	float2 TexC : TEXCOORD0;
};

VertexOut VS(uint vid : SV_VertexID)
{
    VertexOut vout;

    vout.TexC = gTexCoords[vid];

    // 覆盖屏幕的quad在NDC空间中的坐标
    vout.PosH = float4(2.0f*vout.TexC.x - 1.0f, 1.0f - 2.0f*vout.TexC.y, 0.0f, 1.0f);
 
    // 将quad变换至视空间近平面
    float4 ph = mul(vout.PosH, gInvProj);
    vout.PosV = ph.xyz / ph.w;

    return vout;
}

// 计算r遮蔽p的程度
float OcclusionFunction(float distZ)
{
	// 如果r在p的后面，则r无法遮蔽p
	// 如果distZ足够小，则也认为r无法遮蔽p
	// 遮蔽程度的计算函数如下
	//       1.0     -------------\
	//               |           |  \
	//               |           |    \
	//               |           |      \ 
	//               |           |        \
	//               |           |          \
	//               |           |            \
	//  ------|------|-----------|-------------|---------|--> zv
	//        0     Eps          z0            z1        

	float occlusion = 0.0f;
	if(distZ > gSurfaceEpsilon)
	{
		float fadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;
		
		// distZ从gOcclusionFadeStart增加至gOcclusionFadeEnd时，遮蔽线性减少
		occlusion = saturate( (gOcclusionFadeEnd-distZ)/fadeLength );
	}
	
	return occlusion;	
}

float NdcDepthToViewDepth(float z_ndc)
{
    // z_ndc = A + B/viewZ, gProj[2,2]=A，gProj[3,2]=B.
    float viewZ = gProj[3][2] / (z_ndc - gProj[2][2]);
    return viewZ;
}
 
float4 PS(VertexOut pin) : SV_Target
{
	// p -- 计算环境遮蔽的点
	// n -- p点的法向量
	// q -- 始于p的随机偏移向量
	// r -- 潜在遮蔽点

	// 获取该像素的视空间法向量和z坐标
    float3 n = normalize(gNormalMap.SampleLevel(gsamPointClamp, pin.TexC, 0.0f).xyz);
    float pz = gDepthMap.SampleLevel(gsamDepthMap, pin.TexC, 0.0f).r;
    pz = NdcDepthToViewDepth(pz);

	// 重构p的完整的视空间位置(x,y,z)
	// 找到t使得p = t * pin.PosV
	// p.z = t * pin.PosV.z
	// t = p.z / pin.PosV.z
	float3 p = (pz/pin.PosV.z)*pin.PosV;
	
	// 采样随即向量并映射[0,1] --> [-1, +1].
	float3 randVec = 2.0f*gRandomVecMap.SampleLevel(gsamLinearWrap, 4.0f*pin.TexC, 0.0f).rgb - 1.0f;

	float occlusionSum = 0.0f;
	
	// 在p的正半球（由n指定）采样临近点
	for(int i = 0; i < gSampleCount; ++i)
	{
		// 所有的偏移向量都是均匀分布的，但同时也是朝向固定的
		// 将所有偏移向量根据一个随机向量反转
		// 从而得到朝向随机的均匀分布向量
		float3 offset = reflect(gOffsetVectors[i].xyz, randVec);
	
		// 如果向量在(p, n)定义的平面的背面，则取反
		float flip = sign( dot(offset, n) );
		
		// 在遮蔽半径内采样一个点q
		float3 q = p + flip * gOcclusionRadius * offset;
		
		// 计算q的投影纹理坐标
		float4 projQ = mul(float4(q, 1.0f), gProjTex);
		projQ /= projQ.w;

		// 找到沿从眼睛到q的射线的最近深度值（不是q的深度值）
		// 需要从深度贴图中采样
		float rz = gDepthMap.SampleLevel(gsamDepthMap, projQ.xy, 0.0f).r;
        rz = NdcDepthToViewDepth(rz);

		// 重构r的完整的视空间位置(rx,ry,rz)
		// 存在t使得r = t*q
		// r.z = t*q.z ==> t = r.z / q.z
		float3 r = (rz / q.z) * q;
		
		// 测试r是否遮蔽p
		//   * dot(n, normalize(r - p))衡量遮蔽点r在平面(p,n)前面的程度
		//     该值越大，我们赋予越大的遮蔽权重
		//     这也可以防止r和p在同一平面的情况
		//   * 基于p和r之间的距离调整遮蔽权重
		//     如果r距离p过远，则r无法遮蔽p
		float distZ = p.z - r.z;
		float dp = max(dot(n, normalize(r - p)), 0.0f);

        float occlusion = dp*OcclusionFunction(distZ);

		occlusionSum += occlusion;
	}
	
	occlusionSum /= gSampleCount;
	
	float access = 1.0f - occlusionSum;

	// 增加ssao贴图的对比度，舍得ssao的效果更明显
	return saturate(pow(access, 6.0f));
}
