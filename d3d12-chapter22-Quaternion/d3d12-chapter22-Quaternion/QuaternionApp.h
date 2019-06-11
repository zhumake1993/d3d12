#pragma once

#include "Common/D3DApp.h"
#include "Common/MathHelper.h"
#include "Common/UploadBuffer.h"
#include "Common/FrameResource.h"
#include "Common/GeometryGenerator.h"
#include "Common/Camera.h"
#include "AnimationHelper.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// 绘制一个形状需要的参数
struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	XMFLOAT4X4 World = MathHelper::Identity4x4();

	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// 指示对象数据发生变化，需要更新所有FrameResource的对象常量缓冲
	int NumFramesDirty = gNumFrameResources;

	// 对象常量缓冲索引
	UINT ObjCBIndex = -1;

	MeshGeometry* Geo = nullptr;
	Material* Mat = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced的参数
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class QuaternionApp : public D3DApp
{
public:
	QuaternionApp(HINSTANCE hInstance);
	QuaternionApp(const QuaternionApp& rhs) = delete;
	QuaternionApp& operator=(const QuaternionApp& rhs) = delete;
	~QuaternionApp();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateMaterialBuffer(const GameTimer& gt);

	void DefineSkullAnimation();
	void LoadTextures();
	void BuildRootSignature();
	void BuildDescriptorHeaps();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildSkullGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;// 帧资源vector
	FrameResource* mCurrFrameResource = nullptr;// 当前帧资源
	int mCurrFrameResourceIndex = 0;// 当前帧资源索引

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;// 根签名
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;// 顶点输入布局

	// 所有渲染项
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// 根据PSO划分的渲染项
	std::vector<RenderItem*> mOpaqueRitems;

	RenderItem* mSkullRitem = nullptr;
	XMFLOAT4X4 mSkullWorld = MathHelper::Identity4x4();

	PassConstants mMainPassCB;

	Camera mCamera;

	float mAnimTimePos = 0.0f;
	BoneAnimation mSkullAnimation;

	POINT mLastMousePos;
};

