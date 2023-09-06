#include "GameObject.h"
#include "Shader.h"
#include "ResourceManager.h"

CMaterial::CMaterial()
{
	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}
CMaterial::~CMaterial()
{
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}
void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	m_fPitch = m_fYaw = m_fRoll = 0;
	index = -1;
	m_pTextureSRVHeap = nullptr;


}
CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pMaterial) m_pMaterial->Release();
	m_pTextureSRVHeap = nullptr;
	m_pMaterial = nullptr;
}

void CGameObject::SetShader(CShader* pShader)
{
	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}
void CGameObject::SetMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}
void CGameObject::SetMaterial(UINT nReflection)
{
	if (!m_pMaterial) {
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}
	m_pMaterial->m_nReflection = 0;
}
void CGameObject::SetSRVHeap(ID3D12DescriptorHeap* pSRVHeap)
{
	m_pTextureSRVHeap = pSRVHeap;
}
void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CGameObject::ReleaseShaderVariables()
{
}
void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{
}
void CGameObject::OnPrepareRender()
{
}
void CGameObject::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4&
	xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//ī�޶� ��ǥ���� ������ �� ��ǥ��� ��ȯ�Ѵ�. 
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//ī�޶� ��ǥ���� ��(���콺 ��ǥ�� ����ȯ�Ͽ� ���� ��)�� �� ��ǥ��� ��ȯ�Ѵ�. 
	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//������ ���� ���͸� ���Ѵ�. 
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}
int CGameObject::PickObjectByRayIntersection(XMFLOAT3 &xmf3PickPosition, XMFLOAT4X4 &xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//�� ��ǥ���� ������ �����Ѵ�.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//�� ��ǥ���� ������ �޽��� ������ �˻��Ѵ�. 
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}
bool CGameObject::IsVisible(CCamera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
	//�� ��ǥ���� �ٿ�� �ڽ��� ���� ��ǥ��� ��ȯ�Ѵ�. 
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	
	if (pCamera) bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}
void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	if (IsVisible(pCamera)){
		if (m_pTextureSRVHeap){
			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_pTextureSRVHeap->GetGPUDescriptorHandleForHeapStart());
			CResourceManager& temp = CResourceManager::GetInstance();
			if (temp.GetOffset(m_name) >= 0) {
				tex.Offset(temp.GetOffset(m_name), temp.GetSrvSize());
				pd3dCommandList->SetGraphicsRootDescriptorTable(6, tex);
				if (m_pMaterial){
					if (m_pMaterial->m_pShader){
						m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
						m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
					}
				}
				if (m_pMesh) m_pMesh->Render(pd3dCommandList);
			}
		}
		else {
			if (m_pMaterial) {
				if (m_pMaterial->m_pShader) {
					m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
					m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
				}
			}
			if (m_pMesh) m_pMesh->Render(pd3dCommandList);
		}
	}
}

void CGameObject::SetObjectMatrix(XMFLOAT4X4 newObjectMatrix)
{
	m_xmf4x4World = newObjectMatrix;
}
void CGameObject::SetRotate(float fPitch, float fYaw, float fRoll)
{
	m_fPitch = fPitch;
	m_fYaw = fYaw;
	m_fRoll = fRoll;

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, m_fRoll);
	
}
void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
}
//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}
//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�. 
XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}
//���� ��ü�� ���� x-�� �������� �̵��Ѵ�. 
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//���� ��ü�� ���� y-�� �������� �̵��Ѵ�. 
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//���� ��ü�� ���� z-�� �������� �̵��Ѵ�. 
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//���� ��ü�� �־��� ������ ȸ���Ѵ�. 
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	// Create Quaternions for each rotation
	XMVECTOR quaternionX = XMQuaternionRotationRollPitchYaw(fPitch, 0.0f, 0.0f);
	XMVECTOR quaternionY = XMQuaternionRotationRollPitchYaw(0.0f, fYaw, 0.0f);
	XMVECTOR quaternionZ = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, fRoll);

	// Apply the rotations in the desired order
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(quaternionX);
	mtxRotate = Matrix4x4::Multiply(mtxRotate, XMMatrixRotationQuaternion(quaternionY));
	mtxRotate = Matrix4x4::Multiply(mtxRotate, XMMatrixRotationQuaternion(quaternionZ));

	// Apply the final rotation
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}


CCubeObject::CCubeObject() 
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}
CCubeObject::~CCubeObject()
{

}
CSkyAtmosphere::CSkyAtmosphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,CMesh *cpyMesh, ID3D12DescriptorHeap* pSRVHeap)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_name = "Sky";
	SetPosition(0, 0, 0);
	SetSRVHeap(pSRVHeap);
	SetMesh(cpyMesh);
	SetMaterial(1);
	if (m_pMaterial)
	{
		std::wstring ws = L"Sky m_pMaterial Generate!\n";
		OutputDebugString(ws.c_str());
	}

	CShader* pShader = new CSkyShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pMaterial->SetShader(pShader);
}
void CSkyAtmosphere::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
		if (m_pTextureSRVHeap) {
			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_pTextureSRVHeap->GetGPUDescriptorHandleForHeapStart());
			CResourceManager& temp = CResourceManager::GetInstance();
			if (temp.GetOffset(m_name) >= 0) {
				tex.Offset(temp.GetOffset("Sky"), temp.GetSrvSize());
				pd3dCommandList->SetGraphicsRootDescriptorTable(6, tex);
				if (m_pMaterial) {
					if (m_pMaterial->m_pShader) {
						
						m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
						m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
					}
				}
				if (m_pMesh) m_pMesh->Render(pd3dCommandList);
			}
		}
		else {
			if (m_pMaterial) {
				if (m_pMaterial->m_pShader) {
					m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
					m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
				}
			}
			if (m_pMesh) m_pMesh->Render(pd3dCommandList);
		}
}
void CSkyAtmosphere::Animate(float fTimeElapsed) {
	XMFLOAT3 yAxis(0, 1, 0);
	Rotate(&yAxis, -0.3f * fTimeElapsed);
}
void CSkyAtmosphere::Rotate(XMFLOAT3* pxmf3Axis, float fAngle) {

	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis),
		XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
CTerrainObject::CTerrainObject(int nMeshes) {
	m_xmf4x4World = Matrix4x4::Identity();
	
	m_nMeshes = nMeshes;
	m_ppMeshes = nullptr;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = nullptr;
	}
}
CTerrainObject::~CTerrainObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial)
	{
		m_pMaterial->m_pShader->ReleaseShaderVariables();
		m_pMaterial->m_pShader->Release();
	}
}
void CTerrainObject::SetShader(CShader* pShader)
{
	if (!m_pMaterial)
	{
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}
void CTerrainObject::SetMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}
void CTerrainObject::SetMaterial(UINT nReflection)
{
	if (!m_pMaterial) {
		m_pMaterial = new CMaterial();
		m_pMaterial->AddRef();
	}
	m_pMaterial->m_nReflection = 0;
}
void CTerrainObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
	else{
		std::wstring ws = L"m_ppMesh is null!\n";
		OutputDebugStringW(ws.c_str());

	}
}
void CTerrainObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pMaterial) {
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		}
	}
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}
void CTerrainObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}
void CTerrainObject::CreateShaderVariables(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CTerrainObject::ReleaseShaderVariables()
{
}
void CTerrainObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//��ü�� ���� ��ȯ ����� ��Ʈ ���(32-��Ʈ ��)�� ���Ͽ� ���̴� ����(��� ����)�� �����Ѵ�. 
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
XMFLOAT3 CTerrainObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
void CTerrainObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void CTerrainObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CTerrainObject::OnPrepareRender()
{
}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int
	nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CTerrainObject(0)
{

	m_nWidth = nWidth;
	m_nLength = nLength;

	/*���� ��ü�� ���� �޽����� �迭�� ���� ���̴�. 
	nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ���̴�. 
	cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.*/
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	m_xmf3Scale = xmf3Scale;
	m_pHeightMapImage = nullptr;
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);
	
	//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ� ���� ��Ÿ����. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� �����̴�. 
	m_nMeshes = cxBlocks * czBlocks;
	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�. 
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = nullptr;
	CHeightMapGridMesh* pHeightMapGridMesh = nullptr;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ(��ǥ)�̴�. 
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�. 
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart,
			zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}

	SetMaterial(1);

	if (m_pMaterial)
	{
		std::wstring ws = L"Terrain's m_pMaterial Generate!\n";
		OutputDebugString(ws.c_str());
	}
	
	//������ �������ϱ� ���� ���̴��� �����Ѵ�. 
	CShader* pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	m_pMaterial->SetShader(pShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}
