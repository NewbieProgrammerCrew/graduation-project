#pragma once
#include "GameObject.h"

class CGrid : public CGameObject
{
protected:
	XMFLOAT3 m_xmf3Position;
public:
	CGrid();
	CGrid(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual ~CGrid();


	void SetPosition(const XMFLOAT3& xmf3Position) {
		m_xmf3Position = xmf3Position;
	}
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};