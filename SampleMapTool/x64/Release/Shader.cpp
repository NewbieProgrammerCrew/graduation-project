#include "Shader.h"

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) 
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
	}
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* pd3dErrorBlob = nullptr;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	return(d3dShaderByteCode);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다. 
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}


D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = nullptr;
	d3dInputLayoutDesc.NumElements = 0;
	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = nullptr;
	return(d3dShaderByteCode);
}
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = nullptr;
	return(d3dShaderByteCode);
}


//그래픽스 파이프라인 상태 객체를 생성한다. 
void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	ID3DBlob* pd3dVertexShaderBlob = nullptr, * pd3dPixelShaderBlob = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

void CShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
void CShader::ReleaseShaderVariables()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//파이프라인에 그래픽스 상태 객체를 설정한다. 
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}
void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

////////////////////////////////////////////////////////////////////////////////////////

CPlayerShader::CPlayerShader()
{
}
CPlayerShader::~CPlayerShader()
{
}
D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	wchar_t path[] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(path, "VSPlayer", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	wchar_t path[] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(path, "PSPlayer", "ps_5_1",
		ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

}
void CPlayerShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbPlayer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbPlayer->Map(0, NULL, (void**)&m_pcbMappedPlayer);
}
void CPlayerShader::ReleaseShaderVariables()
{
	if (m_pd3dcbPlayer)
	{
		m_pd3dcbPlayer->Unmap(0, NULL);
		m_pd3dcbPlayer->Release();
	}
}
void CPlayerShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	::memcpy(&m_pcbMappedPlayer->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
}
void CPlayerShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

////////////////////////////////////////////////

CObjectsShader::CObjectsShader()
{
}
CObjectsShader::~CObjectsShader()
{
}
D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,24,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	wchar_t path[] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(path, "VSLighting", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	wchar_t path[] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(path, "PSLighting", "ps_5_1", ppd3dShaderBlob));
}
void CObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

}
void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCubeMeshIlluminated* pCubeMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	auto pCubeObject = std::make_shared<CGameObject>();
	pCubeObject -> SetMaterial(m_ppObjects.size() % MAX_MATERIALS);
	pCubeObject->SetMesh(pCubeMesh);
	pCubeObject->SetPosition(0, 10, 0);
	pCubeObject->SetIndex(m_ppObjects.size());
	m_ppObjects.push_back(pCubeObject);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CObjectsShader::BuildCube(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,float x, float y, float z )
{
	CCubeMeshIlluminated* pCubeMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, x, y, z);
	auto pCubeObject = std::make_shared<CGameObject>();
	pCubeObject->SetMaterial(m_ppObjects.size() % MAX_MATERIALS);
	pCubeObject->SetMesh(pCubeMesh);
	pCubeObject->SetIndex(m_ppObjects.size());
	pCubeObject->SetPosition(0, 0, 0);
	m_ppObjects.push_back(pCubeObject);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CObjectsShader::BuildObj(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,const std::string& name, CMesh* cpyMesh, ID3D12DescriptorHeap* m_pSRVHeap)
{
	auto pObject = std::make_shared<CGameObject>();
	pObject->SetName(name);
	pObject->SetMaterial(m_ppObjects.size() % MAX_MATERIALS);
	pObject->SetMesh(cpyMesh);
	pObject->SetPosition(0, 0, 0);
	pObject->SetIndex(m_ppObjects.size());
	pObject->SetSRVHeap(m_pSRVHeap);
	m_ppObjects.push_back(pObject);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void  CObjectsShader::SaveObject(std::ofstream& outFile)
{
	for (int i{}; i < m_ppObjects.size(); ++i){
		m_ppObjects[i]->writeToFile(outFile);
	}
}

void CObjectsShader::ReadObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::ifstream& inFile, ID3D12DescriptorHeap* m_pSRVHeap)
{
	CMesh* Mesh = new CMesh(pd3dDevice, pd3dCommandList);
	auto pObject = std::make_shared<CGameObject>();

	pObject->readToFile(inFile);

	if (!pObject->GetName().empty()) {
		Mesh->readDataToFile(pd3dDevice, pd3dCommandList, inFile);
		pObject->SetIndex(m_ppObjects.size());
		pObject->SetMaterial(1);
		pObject->SetMesh(Mesh);
		pObject->SetSRVHeap(m_pSRVHeap);
		m_ppObjects.push_back(pObject);
	}
}
void CObjectsShader::ResetScene()
{
	while (!m_ppObjects.empty()) {
		m_ppObjects.pop_front();
	}
}
void CObjectsShader::ReleaseObjects()
{
	while (!m_ppObjects.empty()) {
		m_ppObjects.pop_front();
	}
}
void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_ppObjects.size(); ++j)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}
void CObjectsShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		ncbGameObjectBytes * m_ppObjects.size(), D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}

void CObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	XMFLOAT4X4 xmf4x4World;
	for (int j = 0; j < m_ppObjects.size(); j++)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(m_pcbMappedGameObjects + (j * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		
		XMMATRIX identity = XMMatrixIdentity();
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4TexTransform, identity);

		pbMappedcbGameObject->m_nMaterial = m_ppObjects[j]->m_pMaterial->m_nReflection;
	}
}
void CObjectsShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects.Reset();
	}
}
void CObjectsShader::ReleaseUploadBuffers()
{

	for (int j = 0; j < m_ppObjects.size(); ++j) m_ppObjects[j]->ReleaseUploadBuffers();

}
void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_ppObjects.empty())
	{
		CShader::Render(pd3dCommandList, pCamera);
		UpdateShaderVariables(pd3dCommandList);
		UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();
		for (int j = 0; j < m_ppObjects.size(); j++)
		{
			if (m_ppObjects[j])
			{
				pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * j));
				m_ppObjects[j]->Render(pd3dCommandList, pCamera);
			}
		}
	}
}

std::shared_ptr <CGameObject> CObjectsShader::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition,
	XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	std::shared_ptr <CGameObject> pSelectedObject = NULL;
	for (int j = 0; j < m_ppObjects.size(); j++)
	{
		nIntersected = m_ppObjects[j]->PickObjectByRayIntersection(xmf3PickPosition,
			xmf4x4View, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = m_ppObjects[j];
		}
	}
	return(pSelectedObject);
}
void CObjectsShader::RemoveObject(std::shared_ptr <CGameObject> rObject)
{
	auto it = std::find(m_ppObjects.begin(), m_ppObjects.end(), rObject);
	if (it != m_ppObjects.end()) {
		m_ppObjects.erase(it);
		
	}
}
void CObjectsShader::ProcessDeletionQueue()
{
	m_DeletionQueue.clear();
}
void CObjectsShader::AddCopiedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr <CGameObject> cpyObject, ID3D12DescriptorHeap* m_pSRVHeap)
{

	CMesh* cpyMesh(cpyObject->GetMesh());
	CMaterial* cpyMaterial(cpyObject->m_pMaterial);

	auto pCubeObject = std::make_shared<CGameObject>();
	pCubeObject->SetMaterial(cpyMaterial);
	pCubeObject->SetSRVHeap(m_pSRVHeap);
	pCubeObject->SetName(cpyObject->GetName());
	pCubeObject->SetMesh(cpyMesh);
	pCubeObject->SetIndex(m_ppObjects.size());
	pCubeObject->SetObjectMatrix(cpyObject->GetObjectMatrix());
	m_ppObjects.push_back(pCubeObject);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CObjectsShader::SnapToFloor(CGameObject& targetObject)
{

	BoundingOrientedBox xmTargetBoundingBox = targetObject.GetMesh()->GetBoundingBox();
	xmTargetBoundingBox.Transform(xmTargetBoundingBox, XMLoadFloat4x4(&targetObject.m_xmf4x4World));
	BoundingOrientedBox xmBoundingBox2;

	XMFLOAT3 frayStart = xmTargetBoundingBox.Center;
	XMFLOAT3 frayDir = XMFLOAT3(0, -1, 0);

	XMVECTOR rayStart = XMLoadFloat3(&frayStart);
	XMVECTOR rayDir = XMLoadFloat3(&frayDir);
	float closestDistance = FLT_MAX;
	int index = 0;
	for (int i = 0; i < m_ppObjects.size(); ++i)
	{
		if (targetObject.GetIndex() == i) continue;

		if ((*m_ppObjects[i]).GetMesh())
		{
			xmBoundingBox2 = (*m_ppObjects[i]).GetMesh()->GetBoundingBox();
			xmBoundingBox2.Transform(xmBoundingBox2, XMLoadFloat4x4(&(*m_ppObjects[i]).m_xmf4x4World));
			float distance;
			if (xmBoundingBox2.Intersects(rayStart, rayDir, distance) and distance > 0 and distance < closestDistance)
			{
				closestDistance = distance;
				index = i;
			}
		}
	}
	if (closestDistance < FLT_MAX)
	{
		XMFLOAT3 targetPos = targetObject.GetPosition();
		xmBoundingBox2 = (*m_ppObjects[index]).GetMesh()->GetBoundingBox();
		xmBoundingBox2.Transform(xmBoundingBox2, XMLoadFloat4x4(&(*m_ppObjects[index]).m_xmf4x4World));
		targetPos.y = xmBoundingBox2.Center.y +xmBoundingBox2.Extents.y;
		targetObject.SetPosition(targetPos);

	}
	
}



/////////////////////////////////////////////////////////////

CGridShader::CGridShader()
{
}
CGridShader::~CGridShader()
{
}
D3D12_INPUT_LAYOUT_DESC CGridShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CGridShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	wchar_t path[] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(path, "VSGrid", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CGridShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	wchar_t path[] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(path, "PSGrid", "ps_5_1", ppd3dShaderBlob));
}
void CGridShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);

}
void CGridShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_Grid_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGrid = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGrid->Map(0, NULL, (void**)&m_pcbMappedGrid);
}
void CGridShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGrid)
	{
		m_pd3dcbGrid->Unmap(0, NULL);
		m_pd3dcbGrid->Release();
	}
}
void CGridShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	::memcpy(&m_pcbMappedGrid->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
}
void CGridShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbGrid->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(5, d3dGpuVirtualAddress);
}


