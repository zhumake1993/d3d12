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

【D3D初始化基础】

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

【D3D初始化】

1.设备
设备用ID3D12Device表示，设备代表一个显示适配器
设备可用来检测支持的特征等级，创建资源，视图，指令列表等
使用D3D12CreateDevice创建设备：
HRESULT WINAPI D3D12CreateDevice(  
	IUnknown* pAdapter,  // 指定设备要代表的显示适配器，null表示指定主显示适配器
	D3D_FEATURE_LEVEL MinimumFeatureLevel,  // 应用要求支持的最小特征等级
	REFIID riid, // ID3D12Device COM ID 
	void** ppDevice // 返回创建的设备
);
如果创建设备失败，可转为创建WARP设备。WARP设备是一个软件适配器

2.栅栏和描述符大小
CreateFence
GetDescriptorHandleIncrementSize

3.4X多重采样抗锯齿

4.指令队列，指令列表，指令分配器

5.交换链
typedef struct DXGI_SWAP_CHAIN_DESC{  
	DXGI_MODE_DESC BufferDesc;  // 后缓冲的属性
	DXGI_SAMPLE_DESC SampleDesc;  // 多重采样的数量和质量等级
	DXGI_USAGE BufferUsage;  // 指定DXGI_USAGE_RENDER_TARGET_OUTPUT
	UINT BufferCount;  // 缓冲的数量，指定为2
	HWND OutputWindow;  // 输出窗口句柄
	BOOL Windowed;  // 窗口还是全屏
	DXGI_SWAP_EFFECT SwapEffect;  // 指定DXGI_SWAP_EFFECT_FLIP_DISCARD
	UINT Flags;// 指定切换到全屏时如何改变显示模式
} DXGI_SWAP_CHAIN_DESC;

typedef struct DXGI_MODE_DESC{  
	UINT Width;                   // 分辨率宽
	UINT Height;                  // 分辨率高
	DXGI_RATIONAL RefreshRate;  
	DXGI_FORMAT Format;           // 显示格式
	DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;	//渐进的 vs. 交织的
	DXGI_MODE_SCALING Scaling;    // 图像如何在显示器上拉伸
} DXGI_MODE_DESC;

创建交换链
HRESULT IDXGIFactory::CreateSwapChain( 
	IUnknown *pDevice,      // 指向ID3D12CommandQueue的指针
	DXGI_SWAP_CHAIN_DESC *pDesc, // 指向DXGI_SWAP_CHAIN_DESC的指针
	IDXGISwapChain **ppSwapChain // 创建的交换链
);

6.描述符堆
描述符堆用ID3D12DescriptorHeap表示，用CreateDescriptorHeap创建
使用句柄来引用描述符堆中的描述符
GetCPUDescriptorHandleForHeapStart返回描述符堆中的第一个描述符的句柄
CD3DX12_CPU_DESCRIPTOR_HANDLE返回指定句柄：
CD3DX12_CPU_DESCRIPTOR_HANDLE(    
	mRtvHeap->GetCPUDescriptorHandleForHeapStart(), 
	mCurrBackBuffer,   // 偏移  
	mRtvDescriptorSize 	// 描述符的大小
);

7.渲染目标视图
获取交换链中的缓冲资源：
HRESULT IDXGISwapChain::GetBuffer(  
	UINT Buffer,  // 想要获取的缓冲的索引
	REFIID riid,  // ID3D12Resource COM  ID 
	void **ppSurface  // 返回指向代表目标缓冲的ID3D12Resource的指针
);
创建渲染目标视图
void ID3D12Device::CreateRenderTargetView(  
	ID3D12Resource *pResource,  // 指定用作渲染目标的资源
	const D3D12_RENDER_TARGET_VIEW_DESC *pDesc,  // 如果资源有确切类型，指定null
	D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor  // 创建的渲染目标视图的句柄
);

8.深度模板缓冲和视图
深度缓冲是纹理资源，纹理是一种GPU资源，要填充D3D12_RESOURCE_DESC：
typedef struct D3D12_RESOURCE_DESC  {  
	D3D12_RESOURCE_DIMENSION Dimension;  // 资源维度，
	UINT64 Alignment;  
	UINT64 Width;  // 纹理的宽度。对于缓冲资源，改值是缓冲的字节大小
	UINT Height;  // 纹理的高度
	UINT16 DepthOrArraySize;  // 纹理深度，或纹理数组大小（针对1D和2D纹理）
	UINT16 MipLevels;  // mipmap等级。对于深度模板缓冲，只需要1等级
	DXGI_FORMAT Format;  // 格式
	DXGI_SAMPLE_DESC SampleDesc;  // 多重采样数量和质量等级
	D3D12_TEXTURE_LAYOUT Layout;  // 指定D3D12_TEXTURE_LAYOUT_UNKNOWN
	D3D12_RESOURCE_MISC_FLAG MiscFlags;  // 指定D3D12_RESOURCE_MISC_DEPTH_STENCIL
} D3D12_RESOURCE_DESC;

enum D3D12_RESOURCE_DIMENSION  {    
D3D12_RESOURCE_DIMENSION_UNKNOWN = 0,    
D3D12_RESOURCE_DIMENSION_BUFFER = 1,    
D3D12_RESOURCE_DIMENSION_TEXTURE1D = 2,    
D3D12_RESOURCE_DIMENSION_TEXTURE2D = 3,    
D3D12_RESOURCE_DIMENSION_TEXTURE3D = 4  
} D3D12_RESOURCE_DIMENSION;

GPU资源存放在堆中，本质上是拥有特定属性的GPU内存块
使用CreateCommittedResource创建资源，并委托给拥有指定属性的堆：
HRESULT ID3D12Device::CreateCommittedResource(  
	const D3D12_HEAP_PROPERTIES *pHeapProperties,
	D3D12_HEAP_MISC_FLAG HeapMiscFlags,  // 指定D3D12_HEAP_MISC_NONE
	const D3D12_RESOURCE_DESC *pResourceDesc,  
	D3D12_RESOURCE_USAGE InitialResourceState,  // 初始资源状态
	const D3D12_CLEAR_VALUE *pOptimizedClearValue,  // 优化值，指定null表示不指定优化值
	REFIID riidResource,  // ID3D12Resource COM ID
	void **ppvResource // 返回指向ID3D12Resource的指针
);

typedef struct D3D12_HEAP_PROPERTIES { 
	D3D12_HEAP_TYPE     Type; 
	D3D12_CPU_PAGE_PROPERTIES CPUPageProperties; 
	D3D12_MEMORY_POOL    MemoryPoolPreference; 
	UINT CreationNodeMask; 
	UINT VisibleNodeMask;
} D3D12_HEAP_PROPERTIES;

D3D12_HEAP_TYPE：
D3D12_HEAP_TYPE_DEFAULT：资源仅由GPU存取
D3D12_HEAP_TYPE_UPLOAD：从CPU上载至GPU
D3D12_HEAP_TYPE_READBACK：由CPU读取
D3D12_HEAP_TYPE_CUSTOM：高级应用

struct D3D12_CLEAR_VALUE  {    
	DXGI_FORMAT Format;    
	union     
	{      
		FLOAT Color[ 4 ];      
		D3D12_DEPTH_STENCIL_VALUE DepthStencil;    
	};  
}D3D12_CLEAR_VALUE;

资源应尽可能放在默认堆中来获得最佳性能

9.视口
typedef struct D3D12_VIEWPORT {  
	FLOAT TopLeftX;  
	FLOAT TopLeftY;  
	FLOAT Width;  
	FLOAT Height;  
	FLOAT MinDepth;  
	FLOAT MaxDepth;
} D3D12_VIEWPORT;
使用RSSetViewports设置
不能在同一个渲染目标上设置多个视口
当reset指令列表后必须reset视口
可用来实现分屏

10.剪裁矩形
typedef struct tagRECT{  
	LONG  left;  
	LONG  top;  
	LONG  right;  
	LONG  bottom;
} RECT;
使用RSSetScissorRects设置
不能在同一个渲染目标上设置多个剪裁矩形
当reset指令列表后必须reset剪裁矩形

11.计时
在多处理器上，QueryPerformanceCounter可能产生bug，可使用SetThreadAffinityMask来固定处理器

12.其它
清空渲染目标视图
void ID3D12GraphicsCommandList::ClearRenderTargetView(   
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView,  
	const FLOAT ColorRGBA[ 4 ],  // 清空至该颜色
	UINT NumRects,  // pRects中元素的数量
	const D3D12_RECT *pRects // 指定要清空的矩形，nullptr表示整个渲染目标
);

清空深度模板缓冲
void ID3D12GraphicsCommandList::ClearDepthStencilView(   
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView,  
	D3D12_CLEAR_FLAGS ClearFlags,  // 指定深度缓冲，还是模板缓冲，或二者都是
	FLOAT Depth,  // 清空深度缓冲至改值
	UINT8 Stencil,  // 清空模板缓冲至改值
	UINT NumRects,  // pRects中元素的数量
	const D3D12_RECT *pRects  // 指定要清空的矩形，nullptr表示整个渲染目标
);

设置渲染目标
void ID3D12GraphicsCommandList::OMSetRenderTargets(   
	UINT NumRenderTargetDescriptors,  // 要绑定的渲染目标视图的数量
	const D3D12_CPU_DESCRIPTOR_HANDLE *pRenderTargetDescriptors,  // 指向渲染目标视图数组的指针
	BOOL RTsSingleHandleToDescriptorRange,  // 如果pRenderTargetDescriptors中的所有渲染目标视图在描述符堆中是连续的，指定true，否则false
	const D3D12_CPU_DESCRIPTOR_HANDLE *pDepthStencilDescriptor  // 指向深度模板缓冲视图的指针
);

【3D绘图】

1.顶点和输入布局
顶点结构
struct Vertex{  
	XMFLOAT3 Pos;  
	XMFLOAT4 Color;
};

输入布局描述
typedef struct D3D12_INPUT_LAYOUT_DESC{  
	const D3D12_INPUT_ELEMENT_DESC *pInputElementDescs;  
	UINT NumElements;
} D3D12_INPUT_LAYOUT_DESC;

输入元素描述
typedef struct D3D12_INPUT_ELEMENT_DESC{  
	LPCSTR SemanticName;  // 元素名，要与着色器中的签名一致
	UINT SemanticIndex;  // 语义索引
	DXGI_FORMAT Format;  // 格式
	UINT InputSlot;  // 输入槽口索引
	UINT AlignedByteOffset;  // 字节偏移
	D3D12_INPUT_CLASSIFICATION InputSlotClass;  // 指定D3D12_INPUT_PER_VERTEX_DATA
	INT InstanceDataStepRate;  // 指定0
} D3D12_INPUT_ELEMENT_DESC;

2。顶点缓冲
顶点数据要放在一个叫做缓冲的GPU资源中（ID3D12Resource）

包裹类CD3DX12_RESOURCE_DESC
static inline CD3DX12_RESOURCE_DESC Buffer( 
	UINT64 width,    
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,    
	UINT64 alignment = 0 )
{  
	return CD3DX12_RESOURCE_DESC(
		D3D12_RESOURCE_DIMENSION_BUFFER,     
		alignment, width, 1, 1, 1,     DXGI_FORMAT_UNKNOWN, 1, 0,     
		D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags );
}
width是缓冲的字节大小

对于静态几何体，其顶点缓冲可以放在默认堆（D3D12_HEAP_TYPE_DEFAULT）中
需要通过上载缓冲（D3D12_HEAP_TYPE_UPLOAD）来初始化默认缓冲
先将顶点数据从系统内存拷贝至上载缓冲，再讲顶点数据从上载缓冲拷贝至默认缓冲
参考d3dUtil::CreateDefaultBuffer方法

typedef struct D3D12_SUBRESOURCE_DATA{  
	const void *pData;  // 用来初始化缓冲的数据的系统内存指针
	LONG_PTR RowPitch;  // 对于缓冲来说，数据的大小
	LONG_PTR SlicePitch;  // 对于缓冲来说，数据的大小
} D3D12_SUBRESOURCE_DATA;

顶点缓冲视图D3D12_VERTEX_BUFFER_VIEW_DESC，不需要描述符堆
typedef struct D3D12_VERTEX_BUFFER_VIEW
{  
	D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;  // 顶点缓冲的虚地址，用ID3D12Resource::GetGPUVirtualAddress获取
	UINT SizeInBytes;  // 顶点缓冲中从BufferLocation开始的字节大小
	UINT StrideInBytes;  // 每个顶点元素的字节大小
}D3D12_VERTEX_BUFFER_VIEW;

绑定到渲染管线
void ID3D12GraphicsCommandList::IASetVertexBuffers(  
	UINT StartSlot,  // 开始输入槽口
	UINT NumBuffers,  // 顶点缓冲数量
	const D3D12_VERTEX_BUFFER_VIEW *pViews  // 指向顶点缓冲视图数组中第一个元素的指针
);

绘制
void ID3D12CommandList::DrawInstanced(  
	UINT VertexCountPerInstance,  // 要绘制的顶点数量（每个实例）
	UINT InstanceCount,  // 实例数量
	UINT StartVertexLocation,  // 顶点缓冲中开始绘制的第一个顶点的索引
	UINT StartInstanceLocation  // 实例位置
);

3。索引和索引缓冲
索引缓冲视图同样不需要描述符堆
typedef struct D3D12_INDEX_BUFFER_VIEW{
	D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;  // 索引缓冲的虚地址
	UINT SizeInBytes;  // 索引缓冲中从BufferLocation开始的字节大小
	DXGI_FORMAT Format;  // DXGI_FORMAT_R16_UINT（推荐）或者DXGI_FORMAT_R32_UINT
} D3D12_INDEX_BUFFER_VIEW;

使用ID3D12CommandList::SetIndexBuffer将索引缓冲绑定至管线

使用缓冲时，必须使用DrawIndexedInstanced代替DrawInstanced
void ID3D12GraphicsCommandList::DrawIndexedInstanced(  
	UINT IndexCountPerInstance,  // 要绘制的索引数量（每个实例）
	UINT InstanceCount,  // 实例数量
	UINT StartIndexLocation,  // 索引缓冲中标记开始绘制点的索引
	INT BaseVertexLocation,  // 加在索引上的一个整数，也即开始顶点位置
	UINT StartInstanceLocation  // 实例位置
);

4.着色器
透视除由硬件来完成
顶点数据可以提供顶点着色器不需要的额外数据

5.常量缓冲
常量缓冲通常每帧更新一次，因此要用上载缓冲
常量缓冲的大小必须要是最小硬件分配大小（256字节）的整数倍
HLSL中的结构会自动填充至256的整数倍

为了将数据从CPU拷贝至常量缓冲，需要获得指向资源数据的指针
ComPtr<ID3D12Resource> mUploadBuffer;
BYTE* mMappedData = nullptr;
mUploadBuffer->Map(0, nullptr,reinterpret_cast<void**>(&mMappedData));
memcpy(mMappedData, &data, dataSizeInBytes);

使用完常量缓冲后需要unmap
if(mUploadBuffer != nullptr)  
	mUploadBuffer->Unmap(0, nullptr);
mMappedData = nullptr;

6.常量缓冲描述符
创建描述符堆
D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
cbvHeapDesc.NumDescriptors = 1;
cbvHeapDesc.Type =D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
cbvHeapDesc.Flags =D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
cbvHeapDesc.NodeMask = 0;
ComPtr<ID3D12DescriptorHeap> mCbvHeap
md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,  IID_PPV_ARGS(&mCbvHeap));

填充D3D12_CONSTANT_BUFFER_VIEW_DESC
D3D12_CONSTANT_BUFFER_VIEW_DESC::SizeInBytes必须是256的整数倍
D3D12_CONSTANT_BUFFER_VIEW_DESC::OffsetInBytes也必须是256的整数倍
使用ID3D12Device::CreateConstantBufferView创建视图

7.根签名和描述符表
根签名用ID3D12RootSignature表示
根签名是根参数的数组
根参数可以是根常量，跟描述符，描述符表
描述符表指定了描述符堆中的一段连续的描述符

根签名并不做实际的资源绑定
当设置好根签名后，使用SetGraphicsRootDescriptorTable将描述符表绑定到管线
voidID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable(   
	UINT RootParameterIndex,  // 设置的根参数的索引
	D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor  // 设置的描述符表中的第一个描述符的句柄
);

出于性能考虑，尽量减少根签名的体积，尽量减少每帧中修改跟签名的次数

修改根签名后会丢失所有绑定，新的根签名要重新绑定所有的资源

8.编译着色器
HRESULT D3DCompileFromFile(  
	LPCWSTR pFileName,  // hlsl的文件名
	const D3D_SHADER_MACRO *pDefines,  // 指定null
	ID3DInclude *pInclude,  // 指定null
	LPCSTR pEntrypoint,  // 着色器入口的函数名称
	LPCSTR pTarget,  // 指定着色器程序的类型和版本
	UINT Flags1,  // 只能如何编译代码
	UINT Flags2,  // 高级功能
	ID3DBlob **ppCode,  // 返回指向ID3DBlob的指针，其中存储编译好的字节码
	ID3DBlob **ppErrorMsgs  // 返回指向ID3DBlob的指针，其中存储错误信息
);

ID3DBlob是通用内存块，有两个方法：
LPVOID GetBufferPointer：返回指向数据的void* 指针
SIZE_T GetBufferSize：返回缓冲的字节大小

编译着色器并不会将其绑定至管线

9.光栅器
用D3D12_RASTERIZER_DESC表示，用来配置光栅化阶段
typedef struct D3D12_RASTERIZER_DESC {  
	D3D12_FILL_MODE FillMode;   // 默认：D3D12_FILL_SOLID  
	D3D12_CULL_MODE CullMode;   // 默认：D3D12_CULL_BACK  
	BOOL FrontCounterClockwise; // 默认: false  
	INT DepthBias;              // 默认: 0  
	FLOAT DepthBiasClamp;       // 默认: 0.0f
	FLOAT SlopeScaledDepthBias; // 默认: 0.0f  
	BOOL DepthClipEnable;       // 默认: true  
	BOOL ScissorEnable;         // 默认: false  
	BOOL MultisampleEnable;     // 默认: false  
	BOOL AntialiasedLineEnable; // 默认: false  
	UINT ForcedSampleCount;     // 默认: 0  
	D3D12_CONSERVATIVE_RASTERIZATION_MODEConservativeRaster;  // 默认：D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
} D3D12_RASTERIZER_DESC;

10.管线状态对象
typedef struct D3D12_GRAPHICS_PIPELINE_STATE_DESC{  
	ID3D12RootSignature *pRootSignature;  // 根签名
	D3D12_SHADER_BYTECODE VS;  // 顶点着色器
	D3D12_SHADER_BYTECODE PS;  // 像素着色器
	D3D12_SHADER_BYTECODE DS;  // 域着色器
	D3D12_SHADER_BYTECODE HS;  // 壳着色器
	D3D12_SHADER_BYTECODE GS;  // 几何着色器
	D3D12_STREAM_OUTPUT_DESC StreamOutput;  // 高级功能
	D3D12_BLEND_DESC BlendState;  // 指定混合状态
	UINT SampleMask;  // 采样掩码
	D3D12_RASTERIZER_DESC RasterizerState;  // 指定光栅状态
	D3D12_DEPTH_STENCIL_DESC DepthStencilState;  // 指定深度模板状态
	D3D12_INPUT_LAYOUT_DESC InputLayout;  // 指定输入布局
	D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;  // 拓扑
	UINT NumRenderTargets;  // 同时使用的渲染目标状态
	DXGI_FORMAT RTVFormats[8];  // 渲染目标格式
	DXGI_FORMAT DSVFormat;  // 深度模板格式
	DXGI_SAMPLE_DESC SampleDesc;  // 设置多重采样
} D3D12_GRAPHICS_PIPELINE_STATE_DESC;

使用ID3D12Device::CreateGraphicsPipelineState创建管线状态对象

管线状态对象的验证和创建可能很耗时，应该放在初始化阶段

出于性能考虑，尽量不要修改管线状态对象，尤其不要每一次绘图调用都修改管线状态对象

【3D绘图2】

1.帧资源
存储CPU需要修改的资源

2.渲染项
存储绘制一个物体所需要的数据

3.根签名
根签名是跟参数的数组，根参数可以是描述符表，根描述符，根常量
描述符表：引用堆中的连续一段描述符
根描述符：直接指定一个描述符，该描述符不需要在描述符堆中。只有常量缓冲的CBV，缓冲的SRV/UAV可以被绑定为跟描述符，纹理的SRV不能绑定位跟描述符
根常量：直接绑定32位常量值得列表

出于性能要求，根签名至多存放64DWORDs，三种根参数的消耗分别是：
描述符表：1DWORD
根描述符：2DWORDs
根常量：1DWORD每32位常量

描述符表
typedef struct D3D12_DESCRIPTOR_RANGE{  
	D3D12_DESCRIPTOR_RANGE_TYPE RangeType;  // 描述符类型
	UINT NumDescriptors;  // 描述符数量
	UINT BaseShaderRegister;  // 基础着色器注册参数
	UINT RegisterSpace;  // 注册器空间
	UINT OffsetInDescriptorsFromTableStart;  // 距离表开头偏移的描述符的数量
}D3D12_DESCRIPTOR_RANGE;

根描述符
typedef struct D3D12_ROOT_DESCRIPTOR{  
	UINT ShaderRegister;  // 着色器注册参数
	UINT RegisterSpace;  // 注册器空间
}D3D12_ROOT_DESCRIPTOR;
直接使用SetGraphicsRootConstantBufferView绑定到管线
void ID3D12GraphicsCommandList::SetGraphicsRootConstantBufferView(
	UINT RootParameterIndex,  // 根参数的索引
	D3D12_GPU_VIRTUAL_ADDRESS BufferLocation  // 含有常量缓冲数据的资源的虚地址
); 

根常量
typedef struct D3D12_ROOT_CONSTANTS  {    
	UINT ShaderRegister;    // 着色器注册参数
	UINT RegisterSpace;    // 注册器空间
	UINT Num32BitValues;  // 32位常量的数量
}D3D12_ROOT_CONSTANTS;

直接使用SetGraphicsRoot32BitConstants绑定到管线
ID3D12GraphicsCommandList::SetGraphicsRoot32BitConstants(   
	UINT RootParameterIndex,  // 根参数的索引
	UINT Num32BitValuesToSet,  // 32位常量的数量
	const void *pSrcData,  // 指向32位常量的数组的指针
	UINT DestOffsetIn32BitValues  // 偏移的32位常量的数量
);

排列根参数：修改频率高的放在前面
尽量减少根签名的转换

【光照】

1.变换法向量
如果变换矩阵包含分均匀变换和剪切变换，那么需要使用逆转置矩阵
注意，求逆转置矩阵时要清空translation行

2.重要的向量
L：光向量，指向光源
n：法向量
h：光向量和视向量之间的半路向量
A_L：入射环境光的量
B_L：入射直接光的量
m_d：漫反射的光的量
L*n：兰伯特余弦定理
alpha_h：h和L之间的角度
R_F(alpha_h)：基于菲涅尔效应所反射的光的量
m：粗糙度
(n*h)_h：h与n成theta_h的微平面的量
(m+8)/8：规整系数，以确保镜面发射中的能量守恒

3.兰伯特余弦定理
刻画入射光的强度
f(theta) = max(L*n,0)

4.漫反射光
c_d = max(L*n,0)*B_L @ m_d

5.环境光
c_a = A_L @ m_d

6.镜面光
菲涅尔效应：刻画被反射的入射光的比例
R_F(theta_i) = R_F(0)+(1-R_F(0))(1-costheta_i)^5

粗糙度
刻画h与n成theta_h的微平面的数量
s(theta_h) = (m+8)/8 (n*h)^m

c_s = max(L*n,0)*B_L @ R_F(alpha_i) (m+8)/8 (n*h)^m

7.平行光

8.点光
衰变

9.聚光
衰变
k_spot(phi) = max(-L*d,0)^s

【纹理】

1.DDS格式，专用于3D绘图
对于非压缩的数据，用下面的格式：
DXGI_FORMAT_B8G8R8A8_UNORM：低动态范围图像
DXGI_FORMAT_B8G8R8X8_UNORM：低动态范围图像
DXGI_FORMAT_R16G16B16A16_FLOAT：高动态范围图像

压缩的数据的格式：
DXGI_FORMAT_BC1_UNORM
DXGI_FORMAT_BC2_UNORM
DXGI_FORMAT_BC3_UNORM
DXGI_FORMAT_BC4_UNORM
DXGI_FORMAT_BC5_UNORM
DXGI_FORMAT_BC6_UNORM
DXGI_FORMAT_BC7_UNORM

压缩纹理只能用于管线的输入，不能作为渲染目标

由于压缩算法的原因，纹理的维度必须是4的倍数

2.着色器资源描述符
typedef struct D3D12_SHADER_RESOURCE_VIEW_DESC{  
	DXGI_FORMAT Format;  // 数据格式
	D3D12_SRV_DIMENSION ViewDimension;  // 因为要使用2D纹理，指定D3D12_SRV_DIMENSION_TEXTURE2D
	UINT Shader4ComponentMapping;  // 指定D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
	union   
	{    
		D3D12_BUFFER_SRV Buffer;    
		D3D12_TEX1D_SRV Texture1D;    
		D3D12_TEX1D_ARRAY_SRV Texture1DArray;    
		D3D12_TEX2D_SRV Texture2D;    //
		D3D12_TEX2D_ARRAY_SRV Texture2DArray;    
		D3D12_TEX2DMS_SRV Texture2DMS;    
		D3D12_TEX2DMS_ARRAY_SRV Texture2DMSArray;    
		D3D12_TEX3D_SRV Texture3D;    
		D3D12_TEXCUBE_SRV TextureCube;    
		D3D12_TEXCUBE_ARRAY_SRV TextureCubeArray;  
	};
}D3D12_SHADER_RESOURCE_VIEW_DESC;

typedef struct D3D12_TEX2D_SRV
{  
	UINT MostDetailedMip;  // 指定最高纹理映射等级的索引
	UINT MipLevels;  // 从MostDetailedMip开始的纹理映射等级的数量
	UINT PlaneSlice;  // 平面索引
	FLOAT ResourceMinLODClamp;  // 指定最小纹理映射等级，0.0指定所有等级
} D3D12_TEX2D_SRV;

3.过滤器
放大
常量插值法（点过滤）和线性插值法（线性过滤），实际应用中多用线性插值法

缩小，使用纹理映射
纹理映射点过滤和纹理映射线性过滤

各向异性过滤
缓解法向量与视向量接近正交时产生的失真

4.地址模式
wrap：重复图像
border color：将域外的点映射为指定颜色
clamp：将域外的点映射为域内的最近点的颜色
mirror：镜像图像

5.采样器对象
需要绑定到描述符上
typedef struct D3D12_SAMPLER_DESC{  
	D3D12_FILTER Filter;  // 过滤类型
	D3D12_TEXTURE_ADDRESS_MODE AddressU;  // 水平u轴方向的地址模式
	D3D12_TEXTURE_ADDRESS_MODE AddressV;  // 垂直v轴方向的地址模式
	D3D12_TEXTURE_ADDRESS_MODE AddressW;  // 深度w轴方向的地址模式
	FLOAT MipLODBias;  // 纹理映射等级偏移
	UINT MaxAnisotropy;  // 最大各向异性值，范围[1,16]
	D3D12_COMPARISON_FUNC ComparisonFunc;  // 用于阴影映射
	FLOAT BorderColor[ 4 ];  // 指定边界颜色
	FLOAT MinLOD;  // 最小纹理映射等级
	FLOAT MaxLOD;  // 最大纹理映射等级
} D3D12_SAMPLER_DESC;

D3D12_FILTER类型：
D3D12_FILTER_MIN_MAG_MIP_POINT：放大：点过滤，缩小：点过滤
D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT：放大：线性过滤，缩小：点过滤
D3D12_FILTER_MIN_MAG_MIP_LINEAR：放大：线性过滤，缩小：线性过滤
D3D12_FILTER_ANISOTROPIC：各向异性过滤

6.静态采样器
不需要创建描述符堆
至多2032个

CD3DX12_ROOT_SIGNATURE_DESC类的Init方法有两个参数可以设定静态采样器






















































































