#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "d3dUtil.h"
#include "GameTimer.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
protected:

	D3DApp(HINSTANCE hInstance);
	D3DApp(const D3DApp& rhs) = delete;
	D3DApp& operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp();

public:

	static D3DApp* GetApp(); // 获取单例

	HINSTANCE AppInst()const; // 获取应用实例句柄
	HWND      MainWnd()const; // 获取主窗口句柄
	float     AspectRatio()const; // 获取宽高比

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();//创建渲染目标视图描述符堆，深度模板视图描述符堆
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

protected:

	bool InitMainWindow();//初始化主窗口
	bool InitDirect3D();//初始化D3D
	void CreateCommandObjects();//创建指令队列，指令分配器，指令列表
	void CreateSwapChain();//创建交换链

	void FlushCommandQueue();//flush指令队列

	ID3D12Resource* CurrentBackBuffer()const;//返回当前备份缓冲
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;//返回当前备份缓冲视图（描述符）
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;//返回深度模板视图（描述符）

	void CalculateFrameStats();//计算帧状态

	void LogAdapters();//列出所有显示适配器
	void LogAdapterOutputs(IDXGIAdapter* adapter);//列出显示适配器所关联的所有显示输出
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);//列出显示输出所支持的所有显示模式

protected:

	static D3DApp* mApp; // 单例

	HINSTANCE mhAppInst = nullptr; //应用实例句柄
	HWND      mhMainWnd = nullptr; //主窗口句柄
	bool      mAppPaused = false;  //应用是否暂停？
	bool      mMinimized = false;  //应用是否最小化？
	bool      mMaximized = false;  //应用是否最大化？
	bool      mResizing = false;   //是否正在拖动resize bars？
	bool      mFullscreenState = false;//应用是否全屏？

	bool      m4xMsaaState = false; // 是否开启多重采样
	UINT      m4xMsaaQuality = 0; // 多重采样质量等级

	GameTimer mTimer; //计时器

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory; // dxgi工厂
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain; // 交换链
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice; // 设备

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence; // 栅栏
	UINT64 mCurrentFence = 0; // 栅栏当前值

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;//指令队列
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;//指令分配器
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;//指令列表

	static const int SwapChainBufferCount = 2; // 交换链中的缓冲数量
	int mCurrBackBuffer = 0; // 交换链中的当前缓冲
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount]; //渲染目标缓冲
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;//深度模板缓冲

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;//渲染目标视图描述符堆
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;//深度模板视图描述符堆

	D3D12_VIEWPORT mScreenViewport;//视口
	D3D12_RECT mScissorRect;//剪裁矩形

	UINT mRtvDescriptorSize = 0;//渲染目标视图描述符的大小
	UINT mDsvDescriptorSize = 0;//深度模板视图描述符的大小
	UINT mCbvSrvUavDescriptorSize = 0;//常量缓冲视图描述符，着色器资源视图描述符，无序存取视图描述符的大小

	//派生类应该在构造函数中定制以下变量
	std::wstring mMainWndCaption = L"d3d App"; // 主窗口标题
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE; // 好像并没有用到。。。。
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // 后缓冲格式
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 深度模板缓冲格式
	int mClientWidth = 800; // 屏幕宽
	int mClientHeight = 600; // 屏幕高
};

