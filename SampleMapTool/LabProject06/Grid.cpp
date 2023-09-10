#include "Grid.h"
#include "Shader.h"

CGrid::CGrid()
{
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
}
CGrid::~CGrid()
{
	ReleaseShaderVariables();
	
}
void CGrid::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CGrid::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();
}
void CGrid::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}




void CGrid::OnPrepareRender()
{
	
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}

void CGrid::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}

CGrid::CGrid(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CGridMesh* pAirplaneMesh = new CGridMesh(pd3dDevice, pd3dCommandList);
	SetMesh(pAirplaneMesh);
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CGridShader* pShader = new CGridShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetShader(pShader);
}
