#pragma once

#include "Common/D3DApp.h"
#include "Common/MathHelper.h"
#include "Common/UploadBuffer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class BoxApp : public D3DApp
{
public:
	BoxApp(HINSTANCE hInstance);
	BoxApp(const BoxApp& rhs) = delete;
	BoxApp& operator=(const BoxApp& rhs) = delete;
	~BoxApp();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;// ��ǩ��
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;// ����������������

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;// �洢�����������ػ���

	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;// ������ֿ�

	ComPtr<ID3DBlob> mvsByteCode = nullptr;// ������ɫ���ֽ���
	ComPtr<ID3DBlob> mpsByteCode = nullptr;// ������ɫ���ֽ���

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;// �������벼��

	ComPtr<ID3D12PipelineState> mPSO = nullptr;// ����״̬����

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;

	POINT mLastMousePos;
};

