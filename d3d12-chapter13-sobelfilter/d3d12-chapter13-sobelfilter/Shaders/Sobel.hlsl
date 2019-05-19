//=============================================================================
// 使用Sobel算子进行边缘检测
//=============================================================================

Texture2D gInput            : register(t0);
RWTexture2D<float4> gOutput : register(u0);


// 从RGB值近似计算亮度，这些权重基于实验，实验基于眼睛对光的不同波长的敏感度
float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

[numthreads(16, 16, 1)]
void SobelCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
    // 采样该像素的周围像素
	float4 c[3][3];
	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			int2 xy = dispatchThreadID.xy + int2(-1 + j, -1 + i);
			c[i][j] = gInput[xy]; 
		}
	}

	// 对于每个颜色通道，使用Sobel估计x偏导
	float4 Gx = -1.0f*c[0][0] - 2.0f*c[1][0] - 1.0f*c[2][0] + 1.0f*c[0][2] + 2.0f*c[1][2] + 1.0f*c[2][2];

	// 对于每个颜色通道，使用Sobel估计y偏导
	float4 Gy = -1.0f*c[2][0] - 2.0f*c[2][1] - 1.0f*c[2][1] + 1.0f*c[0][0] + 2.0f*c[0][1] + 1.0f*c[0][2];

	// 梯度是(Gx, Gy)，对于每个颜色通道，计算magnitude来得到最大变化率
	float4 mag = sqrt(Gx*Gx + Gy*Gy);

	// 边缘黑色，非边缘白色
	mag = 1.0f - saturate(CalcLuminance(mag.rgb));

	gOutput[dispatchThreadID.xy] = mag;
}
