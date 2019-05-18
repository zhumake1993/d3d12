cbuffer cbSettings : register(b0)
{
	// 常量缓冲中不能有数组项，因此需要列出每个元素
	
	int gBlurRadius;

	// 11个模糊权重
	float w0;
	float w1;
	float w2;
	float w3;
	float w4;
	float w5;
	float w6;
	float w7;
	float w8;
	float w9;
	float w10;
};

static const int gMaxBlurRadius = 5;


Texture2D gInput            : register(t0);
RWTexture2D<float4> gOutput : register(u0);

#define N 256
#define CacheSize (N + 2*gMaxBlurRadius)
groupshared float4 gCache[CacheSize];

[numthreads(N, 1, 1)]
void HorzBlurCS(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

	// 使用共享内存来减少带宽
	// 模糊N个像素，需要载入N + 2*BlurRadius个像素
	
	// 该线程组运行N个线程
	// 为了获得额外的2*BlurRadius个像素，需要2*BlurRadius个线程来采样额外的像素
	if(groupThreadID.x < gBlurRadius) // 最左边的gBlurRadius个线程
	{
		// 超过图像边缘
		int x = max(dispatchThreadID.x - gBlurRadius, 0);
		gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	if(groupThreadID.x >= N-gBlurRadius) // 最右边的gBlurRadius个线程
	{
		// 超过图像边缘
		int x = min(dispatchThreadID.x + gBlurRadius, gInput.Length.x-1);
		gCache[groupThreadID.x+2*gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];
	}

	gCache[groupThreadID.x+gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy-1)];

	// 等待其他线程完成
	GroupMemoryBarrierWithGroupSync();
	
	//
	// 模糊像素
	//

	float4 blurColor = float4(0, 0, 0, 0);
	
	for(int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + gBlurRadius + i;
		
		blurColor += weights[i+gBlurRadius]*gCache[k];
	}
	
	gOutput[dispatchThreadID.xy] = blurColor;
}

[numthreads(1, N, 1)]
void VertBlurCS(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

	// 使用共享内存来减少带宽
	// 模糊N个像素，需要载入N + 2*BlurRadius个像素

	// 该线程组运行N个线程
	// 为了获得额外的2*BlurRadius个像素，需要2*BlurRadius个线程来采样额外的像素
	if(groupThreadID.y < gBlurRadius) // 最上边的gBlurRadius个线程
	{
		// 超过图像边缘
		int y = max(dispatchThreadID.y - gBlurRadius, 0);
		gCache[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}
	if(groupThreadID.y >= N-gBlurRadius) // 最下边的gBlurRadius个线程
	{
		// 超过图像边缘
		int y = min(dispatchThreadID.y + gBlurRadius, gInput.Length.y-1);
		gCache[groupThreadID.y+2*gBlurRadius] = gInput[int2(dispatchThreadID.x, y)];
	}
	
	gCache[groupThreadID.y+gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy-1)];


	// 等待其他线程完成
	GroupMemoryBarrierWithGroupSync();
	
	//
	// 模糊像素
	//

	float4 blurColor = float4(0, 0, 0, 0);
	
	for(int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;
		
		blurColor += weights[i+gBlurRadius]*gCache[k];
	}
	
	gOutput[dispatchThreadID.xy] = blurColor;
}