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

// ����һ����״��Ҫ�Ĳ���
struct RenderItem
{
	RenderItem() = default;
	RenderItem(const RenderItem& rhs) = delete;

	XMFLOAT4X4 World = MathHelper::Identity4x4();

	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	// ָʾ�������ݷ����仯����Ҫ��������FrameResource�Ķ���������
	int NumFramesDirty = gNumFrameResources;

	// ��������������
	UINT ObjCBIndex = -1;

	MeshGeometry* Geo = nullptr;
	Material* Mat = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced�Ĳ���
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

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;// ֡��Դvector
	FrameResource* mCurrFrameResource = nullptr;// ��ǰ֡��Դ
	int mCurrFrameResourceIndex = 0;// ��ǰ֡��Դ����

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;// ��ǩ��
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;// �������벼��

	// ������Ⱦ��
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// ����PSO���ֵ���Ⱦ��
	std::vector<RenderItem*> mOpaqueRitems;

	RenderItem* mSkullRitem = nullptr;
	XMFLOAT4X4 mSkullWorld = MathHelper::Identity4x4();

	PassConstants mMainPassCB;

	Camera mCamera;

	float mAnimTimePos = 0.0f;
	BoneAnimation mSkullAnimation;

	POINT mLastMousePos;
};

