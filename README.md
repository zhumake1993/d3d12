# d3d12
learn how to use d3d12

【Tip】
输出调试信息：OutputDebugString(text.c_str());

【项目初始化】
1.64位
2.常规-字符集-使用Unicode字符集
3.C/C++-代码生成-浮点模型-快速(fp:fast)
4.连接器-系统-子系统-窗口

【向量代数】

1.XMVECTOR与XMFLOATn
使用XMVECTOR存储局部和全局变量
使用XMFLOATn存储类的数据成员
使用XMVECTOR进行数学计算

2.XMVECTOR作为参数
前三个XMVECTOR参数的类型应为FXMVECTOR
第四个参数的类型是GXMVECTOR
第五和第六个参数的类型是HXMVECTOR
其它参数的类型是CXMVECTOR
函数返回值不需要遵从此原则

3.常数向量
使用XMVECTORF32构造常数向量

4.浮点数不能直接比较相等性

【矩阵代数】

1.XMMATRIX与XMFLOAT4X4
使用XMMATRIX存储局部和全局变量
使用XMFLOAT4X4存储类的数据成员
使用XMMATRIX进行数学计算

2.XMMATRIX作为参数
第一个参数的类型是FXMMATRIX
其它参数的类型是CXMMATRIX

【变换】

1.用(x,y,z,0)表示向量，用(x,y,z,1)表示点

【D3D初始化】

1.组件对象模型（COM）
Microsoft::WRL::ComPtr可以看作是针对COM的智能指针，有三个主要方法：
Get：返回指向COM的指针
GetAddressOf：返回指向COM的指针的地址
Reset；等价于赋值nullptr，使引用计数减一
都有一个前缀I，如ID3D12GraphicsCommandList

2.DXGI_FORMAT，数据格式，用于纹理，顶点数据，顶点索引等
DXGI_FORMAT_R32G32B32_FLOAT：32位浮点
DXGI_FORMAT_R16G16B16A16_UNORM：16位[0,1]
DXGI_FORMAT_R32G32_UINT：32位无符号整数
DXGI_FORMAT_R8G8B8A8_UNORM：8位[0,1]
DXGI_FORMAT_R8G8B8A8_SNORM：8位[-1,1]
DXGI_FORMAT_R8G8B8A8_SINT：8位[-128,127]
DXGI_FORMAT_R8G8B8A8_UINT：8位[0,255]

3.交换链的前后缓冲是纹理

4.深度缓冲是纹理，可用格式如下：
DXGI_FORMAT_D32_FLOAT_S8X24_UINT：32位浮点用作深度缓冲，8位无符号整数[0,255]用作模板缓冲，24位用作填充
DXGI_FORMAT_D32_FLOAT：32位浮点深度缓冲
DXGI_FORMAT_D24_UNORM_S8_UINT：24位[0,1]用作深度缓冲，8位[0,255]用作模板缓冲
DXGI_FORMAT_D16_UNORM：16位[0,1]用作深度缓冲

5.使用描述符将资源绑定到渲染管线上，描述符等价于视图

6.描述符的类型：
CBV/SRV/UAV：常量缓冲，着色器资源，无序存取视图资源
Sampler：采样器资源，用于纹理
RTV：渲染目标资源
DSV：深度模板资源

7.描述符堆是描述符的数组。不同类型的描述符不能存储在同一个描述符堆中
创建描述符的代价较大，应该在初始化时创建描述符
尽可能不要使用无类型资源

8.两种抗锯齿技术：超级采样和多重采样
超级采样代价过大，实际中更多地使用多重采样

9.D3D中的多重采样
采样结构体：
typedef struct DXGI_SAMPLE_DESC
{  
	UINT Count;		每像素的采样数量
	UINT Quality;	质量等级（具体意义依硬件厂商而定），范围依赖于纹理格式和Count
} DXGI_SAMPLE_DESC;
给定纹理格式和采样数量，查询质量等级：
typedef struct D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS { 
	DXGI_FORMAT Format; 
	UINT SampleCount; 
	D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG Flags; 
	UINT NumQualityLevels;
} D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS；

D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
msQualityLevels.Format = mBackBufferFormat;
msQualityLevels.SampleCount = 4;
msQualityLevels.Flags =D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
msQualityLevels.NumQualityLevels = 0;
ThrowIfFailed(md3dDevice->CheckFeatureSupport(
	D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,  
	&msQualityLevels,  
	sizeof(msQualityLevels)));
合法的质量等级的范围是[0,NumQualityLevels–1]
采样数量的最大值定义为：
#define    D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT   ( 32 )
采样数量通常设为4或8
如果不想使用多重采样，将采样数量设为1，并把质量等级设为0
所有支持D3D11的设备都支持4倍多重采样，不论渲染目标目标格式
交换链缓冲和深度缓冲都需要DXGI_SAMPLE_DESC，并且多重采样的设定要保持一致

10.特征等级：D3D_FEATURE_LEVEL枚举类型。暂时假定支持D3D_FEATURE_LEVEL_11_0

11.Direcx图形基础设施（DXGI）
显示适配器：通常是物理硬件，如显卡。系统也会有一个软件显示适配器来模拟硬件图形功能
一个系统可以有多个显示适配器，显示适配器用IDXGIAdapter接口表示
使用EnumAdapters查询显示适配器

一个系统可以有多个显示器，显示器是显示输出的一个例子，显示输出用IDXGIOutput接口表示
一个显示适配器可以关联多个显示输出
使用EnumOutputs查询新时输出
“Microsoft Basic Render Driver”没有显示输出

每个显示器支持多种显示模式，显示模式用DXGI_MODE_DESC表示：
typedef struct DXGI_MODE_DESC{  
	UINT Width;                   // 分辨率宽
	UINT Height;                  // 分辨率高
	DXGI_RATIONAL RefreshRate;  
	DXGI_FORMAT Format;           // 显示格式
	DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;	//渐进的 vs. 交织的
	DXGI_MODE_SCALING Scaling;    // 图像如何在显示器上拉伸
} DXGI_MODE_DESC;

typedef struct DXGI_RATIONAL{  
	UINT Numerator;  
	UINT Denominator;
} DXGI_RATIONAL;

typedef enum DXGI_MODE_SCANLINE_ORDER{  
	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED    = 0,  
	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE    = 1,
	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST = 2,  
	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST = 3
} DXGI_MODE_SCANLINE_ORDER;

typedef enum DXGI_MODE_SCALING{  
	DXGI_MODE_SCALING_UNSPECIFIED  = 0,  
	DXGI_MODE_SCALING_CENTERED   = 1,  
	DXGI_MODE_SCALING_STRETCHED   = 2
} DXGI_MODE_SCALING;
固定显示模式格式，可以查询一个显示输出所支持的所有显示模式，使用GetDisplayModeList

12.CheckFeatureSupport函数
HRESULT ID3D12Device::CheckFeatureSupport(  
D3D12_FEATURE Feature,		// 要检测的特征
void *pFeatureSupportData,	// 指向用以检索特征支持信息的数据结构的指针
UINT FeatureSupportDataSize	// pFeatureSupportData所指数据结构的大小
);

使用CheckFeatureSupport函数查询特征等级
typedef struct D3D12_FEATURE_DATA_FEATURE_LEVELS { 
	UINT          NumFeatureLevels; 
	const D3D_FEATURE_LEVEL *pFeatureLevelsRequested; 
	D3D_FEATURE_LEVEL   MaxSupportedFeatureLevel;
} D3D12_FEATURE_DATA_FEATURE_LEVELS;

D3D_FEATURE_LEVEL featureLevels[3] =
{  
	D3D_FEATURE_LEVEL_11_0, // 首先检查D3D11
	D3D_FEATURE_LEVEL_10_0, // 然后检查D3D10
	D3D_FEATURE_LEVEL_9_3  // 最后检查D3D9.3
};

D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;
featureLevelsInfo.NumFeatureLevels = 3;
featureLevelsInfo.pFeatureLevelsRequested = featureLevels;
md3dDevice->CheckFeatureSupport(  
	D3D12_FEATURE_FEATURE_LEVELS,  
	&featureLevelsInfo,  
	sizeof(featureLevelsInfo));
函数通过MaxSupportedFeatureLevel返回最大支持的特征等级

13.CPU/GPU同步
指令队列用ID3D12CommandQueue表示
通过填充D3D12_COMMAND_QUEUE_DESC并调用CreateCommandQueue来创建指令队列

方法ExecuteCommandLists将指令添加到指令队列中
图形指令列表用ID3D12GraphicsCommandList表示，其继承自ID3D12CommandList
通过Close方法指示指令添加完毕，该方法必须在ExecuteCommandLists前执行
指令列表通过CreateCommandList创建：
HRESULT ID3D12Device::CreateCommandList(
	UINT nodeMask,  // 指示相关联的GPU，0表示单GPU系统
	D3D12_COMMAND_LIST_TYPE type,  // 指令列表的类型
	ID3D12CommandAllocator *pCommandAllocator,  // 相关联的指令分配器，类型必须一致
	ID3D12PipelineState *pInitialState,  // 指令列表的初始管线状态
	REFIID riid,  // ID3D12CommandList的COM ID
	void **ppCommandList  // 输出一个指向被创建的指令列表的指针
);

与一个指令列表相关联的是一个指令分配器，用ID3D12CommandAllocator表示
当指令被加入到指令列表中，它们实际上被存储在相关联的指令分配器中
ExecuteCommandLists执行后，指令列表将会引用指令分配器中的指令
指令分配器通过CreateCommandAllocator创建：
HRESULT ID3D12Device::CreateCommandAllocator(   
	D3D12_COMMAND_LIST_TYPE type,	// 相关联的指令列表的类型
	REFIID riid,  					// ID3D12CommandAllocator的COM ID
	void **ppCommandAllocator		// 输出一个指向被创建的指令分配器的指针
);

可以使用GetNodeCount来查询GPU适配器节点的数量

一个指令分配器可以关联多个指令列表，但只能同时使用一个指令列表来记录指令
当指令列表被创建或重置时，其处于open状态

当调用ExecuteCommandList(C)后，就可以通过调用Reset方法来重用C的内存
HRESULT ID3D12CommandList::Reset(   
	ID3D12CommandAllocator *pAllocator,	// 相关联的指令分配器，类型必须一致
	ID3D12PipelineState *pInitialState	// 指令列表的初始管线状态
);
重置指令列表并不影响指令队列中的指令，因为指令分配器在内存中仍然拥有指令队列所引用的指令

Reset方法可以重用指令分配器的内存。但是，由于指令队列仍然有可能引用指令分配器中的指令
在确保GPU执行完指令分配器中的所有指令之前，不能reset指令分配器

栅栏被用来同步CPU/GPU，用ID3D12Fence表示，使用CreateFence创建

14.资源过渡
使用过渡资源分界线，用CD3DX12_RESOURCE_BARRIER表示
使用ResourceBarrier来过渡资源





































