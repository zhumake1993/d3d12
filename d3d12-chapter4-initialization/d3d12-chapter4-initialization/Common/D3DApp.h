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

	static D3DApp* GetApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;

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

	void LogAdapters();//列出所有适配器
	void LogAdapterOutputs(IDXGIAdapter* adapter);//列出适配器所关联的输出（显示器），一个适配器可以关联多个输出
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);//列出输出所支持的显示模式，一个输出可以支持多种显示模式

protected:

	static D3DApp* mApp;

	HINSTANCE mhAppInst = nullptr; //应用实例句柄
	HWND      mhMainWnd = nullptr; //主窗口句柄
	bool      mAppPaused = false;  //应用是否暂停？
	bool      mMinimized = false;  //应用是否最小化？
	bool      mMaximized = false;  //应用是否最大化？
	bool      mResizing = false;   //是否正在拖动resize bars？
	bool      mFullscreenState = false;//应用是否全屏？

	bool      m4xMsaaState = false;
	UINT      m4xMsaaQuality = 0;

	GameTimer mTimer;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;//指令队列
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;//指令分配器
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;//指令列表

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;//渲染目标视图描述符堆
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;//深度模板视图描述符堆

	D3D12_VIEWPORT mScreenViewport;//视口
	D3D12_RECT mScissorRect;//剪裁矩形

	UINT mRtvDescriptorSize = 0;//渲染目标视图描述符的大小
	UINT mDsvDescriptorSize = 0;//深度模板视图描述符的大小
	UINT mCbvSrvUavDescriptorSize = 0;//常量缓冲视图描述符，着色器资源视图描述符，无序存取视图描述符的大小

	//派生类应该在构造函数中定制以下变量
	std::wstring mMainWndCaption = L"d3d App";
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800;
	int mClientHeight = 600;
};

