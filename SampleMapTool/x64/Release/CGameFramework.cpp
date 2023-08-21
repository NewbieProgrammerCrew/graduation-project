#include <cmath>
#include "stdafx.h"
#include "CGameFramework.h"
#include "ResourceManager.h"

CGameFramework::CGameFramework() 
{

	currentTransformMode = TRANSLATE;

	m_pdxgiFactory = nullptr;
	m_pdxgiSwapChain = nullptr;
	m_pd3dDevice = nullptr;

	m_pd3dCommandAllocator = nullptr;
	
	m_pd3dCommandQueue = nullptr;
	m_pd3dPipelineState = nullptr;
	m_pd3dCommandList = nullptr;

	for (int i = 0; i < m_nSwapChainBuffers; ++i) m_ppd3dRenderTargetBuffers[i] = nullptr;

	m_TextureSRV = nullptr;
	m_pd3dSrvDescriptorImGUIHeap = nullptr;

	m_pd3dRtvDescriptorHeap = nullptr;
	m_nRtvDescriptorIncrementSize = 0;

	m_pd3dDepthStencilBuffer = nullptr;
	m_pd3dDsvDescriptorHeap = nullptr;
	m_nDsvDescriptorIncrementSize = 0;

	m_nSwapChainBufferIndex = 0;

	m_hFenceEvent = nullptr;
	m_pd3dFence = nullptr;
	
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	m_pScene = nullptr;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
	_tcscpy_s(m_pszFrameRate, _T("NPC MapTool ("));
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) 
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D디바이스 명령 큐와 명령 리스트, 스왑 체인등을 생성하는 함수를 호출한다.

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();
	BuildObjects();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	IMGUI_CHECKVERSION();
	
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(hMainWnd);
	ImGui_ImplDX12_Init(m_pd3dDevice, m_nSwapChainBuffers,
		DXGI_FORMAT_R8G8B8A8_UNORM, m_pd3dSrvDescriptorImGUIHeap,
		m_pd3dSrvDescriptorImGUIHeap->GetCPUDescriptorHandleForHeapStart(),
		m_pd3dSrvDescriptorImGUIHeap->GetGPUDescriptorHandleForHeapStart());
	ImGuizmo::SetRect(0, 0, m_nWndClientWidth, m_nWndClientHeight);
	return true;
	 
}
void CGameFramework::OnDestroy() 
{
	waitForGpuComplete();
	//GPU가 모든 명령 리스트를 실행할 때까지 기다린다.

	ReleaseObjects();
	//게임 객체(게임 월드 객체)를 소멸한다.
	::CloseHandle(m_hFenceEvent);

	for (int i = 0; i < m_nSwapChainBuffers; ++i) if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();
	if (m_TextureSRV) m_TextureSRV->Release();
	if (m_pd3dSrvBuffer) m_pd3dSrvBuffer->Release();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();



	while (!importObj.empty())
	{
		importObj.pop_front();
	}


#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif

}
void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다. 
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
}
void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;
	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), 
		(void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);
	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i,&pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void**)&m_pd3dDevice))) break;
	}
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다. 
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다. 
	
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence),(void**)&m_pd3dFence);
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
 
	if (pd3dAdapter) pd3dAdapter->Release();
}
void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, 
		_uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);
	//직접(Direct) 명령 큐를 생성한다.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);
	//직접(Direct) 명령 할당자를 생성한다.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pd3dCommandAllocator,NULL,__uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//직접(Direct) 명령 리스트를 생성한다.

	hResult = m_pd3dCommandList->Close();
	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌 상태로 만든다.

}
void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다.
	m_nRtvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//렌더 타겟 서술자 힙의 원소의 크기를 저장한다.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	//깊이 - 스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	
	m_nDsvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.


	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 5;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_TextureSRV));
	m_nTextureSRVSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	m_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pd3dSrvDescriptorImGUIHeap));
	m_nSrvDescriptorIncrementSize =
		m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	CResourceManager& resourceManager = CResourceManager::GetInstance();
	resourceManager.SetSrvSize(m_nTextureSRVSize);
}	

//스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰를 생성한다.
void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; ++i) { m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource),  (void**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}
void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));

	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource),(void**)&m_pd3dDepthStencilBuffer);
	//깊이-스텐실 버퍼를 생성한다.

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);

}
void CGameFramework::CreateShaderResourceView()
{
	
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	CResourceManager& resourceManager = CResourceManager::GetInstance();
	

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_TextureSRV->GetCPUDescriptorHandleForHeapStart());

	ComPtr <ID3D12Resource>  resource1 = resourceManager.GetTexture("wall");
	srvDesc.Format = resource1->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = resource1->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_pd3dDevice->CreateShaderResourceView(resource1.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, m_nTextureSRVSize);
	resource1 = resourceManager.GetTexture("chest");
	srvDesc.Format = resource1->GetDesc().Format;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = resource1->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_pd3dDevice->CreateShaderResourceView(resource1.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, m_nTextureSRVSize);
	resource1 = resourceManager.GetTexture("box");
	srvDesc.Format = resource1->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = resource1->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_pd3dDevice->CreateShaderResourceView(resource1.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, m_nTextureSRVSize);
	resource1 = resourceManager.GetTexture("grass");
	srvDesc.Format = resource1->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = resource1->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_pd3dDevice->CreateShaderResourceView(resource1.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, m_nTextureSRVSize);
	resource1 = resourceManager.GetTexture("default");
	srvDesc.Format = resource1->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = resource1->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_pd3dDevice->CreateShaderResourceView(resource1.Get(), &srvDesc, hDescriptor);

}
void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	OBJMesh* temp = new OBJMesh(m_pd3dDevice, m_pd3dCommandList, "CaveWalls4_A.obj");
	importObj.push_back(temp);
	temp = new OBJMesh(m_pd3dDevice, m_pd3dCommandList, "treasure_chest.obj");
	importObj.push_back(temp);
	temp = new OBJMesh(m_pd3dDevice, m_pd3dCommandList, "flat.obj");
	importObj.push_back(temp);
	temp = new OBJMesh(m_pd3dDevice, m_pd3dCommandList, "cube.obj");
	importObj.push_back(temp);

	CResourceManager& resourceManager = CResourceManager::GetInstance();
	resourceManager.LoadTexture(m_pd3dDevice, m_pd3dCommandList, "wall", "wall.dds", 0);
	resourceManager.LoadTexture(m_pd3dDevice, m_pd3dCommandList, "chest", "treasure_chest.dds", 1);
	resourceManager.LoadTexture(m_pd3dDevice, m_pd3dCommandList, "box", "box.dds", 2);
	resourceManager.LoadTexture(m_pd3dDevice, m_pd3dCommandList, "grass", "Grass.dds", 3);
	resourceManager.LoadTexture(m_pd3dDevice, m_pd3dCommandList, "default", "default.dds", 4);
	CreateShaderResourceView();

	m_pScene = new CScene();
	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	
	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
	m_pScene->m_pPlayer = m_pPlayer = pAirplanePlayer;
	m_pCamera = m_pPlayer->GetCamera();
	CGrid* pGrid = new CGrid(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
	m_pGrid = pGrid;
	m_pd3dCommandList->Close();
	
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	waitForGpuComplete();
	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();
	if (m_pGrid) m_pGrid->ReleaseUploadBuffers();
	m_GameTimer.Reset();
}
void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}

///////프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수///////
void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		
		if (GetAsyncKeyState('W') & 0x8000) dwDirection |= DIR_FORWARD;
		if (GetAsyncKeyState('S') & 0x8000) dwDirection |= DIR_BACKWARD;
		if (GetAsyncKeyState('A') & 0x8000) dwDirection |= DIR_LEFT;
		if (GetAsyncKeyState('D') & 0x8000) dwDirection |= DIR_RIGHT;
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) dwDirection |= DIR_UP;
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) dwDirection |= DIR_DOWN;

	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (::GetCapture() == m_hWnd)
	{
		::SetCursor(NULL);
		::GetCursorPos(&ptCursorPos);
	
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f)){
		if (m_pSelectedObject){
			ProcessSelectedObject(dwDirection, cxDelta, cyDelta);
		}
		else{
			if (cxDelta || cyDelta){
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			if (dwDirection) 
				m_pPlayer->Move(dwDirection, 50.0f * m_GameTimer.GetTimeElapsed(), false);
		}
	}
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다. 
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}
void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}
XMVECTOR EulerToQuaternion(float pitch, float yaw, float roll)
{
	
	const float DEG_TO_RAD = (3.14159265f / 180.0f);
	pitch *= DEG_TO_RAD;
	yaw *= DEG_TO_RAD;
	roll *= DEG_TO_RAD;


	XMVECTOR qRoll = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), roll);
	XMVECTOR qPitch = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), pitch);
	XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), yaw);


	XMVECTOR combined = XMQuaternionMultiply(qYaw, XMQuaternionMultiply(qPitch, qRoll));

	return combined;
}
void QuaternionToEuler(const XMVECTOR& quaternion, float& yaw, float& roll, float& pitch)
{

	float q0 = XMVectorGetW(quaternion);
	float q1 = XMVectorGetX(quaternion);
	float q2 = XMVectorGetY(quaternion);
	float q3 = XMVectorGetZ(quaternion);

	roll = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3));
	yaw = asinf(2.0f * (q0 * q2 - q3 * q1));
	pitch = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2));

	// Convert to degrees
	const float RAD_TO_DEG = (180.0f / 3.14159265f);
	pitch *= RAD_TO_DEG;
	yaw *= RAD_TO_DEG;
	roll *= RAD_TO_DEG;
}
void ClampEulerAngles(float& angle)
{
	while (angle > 360.0f) angle -= 360.0f;
	while (angle < 0.0f) angle += 360.0f;
}


void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f);
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse)
		ProcessInput();
	AnimateObjects();

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	switch (currentTransformMode) {
	case TRANSLATE:
		operation = ImGuizmo::TRANSLATE;
		for (int i{}; i < 3; ++i)
			snapValue[i] = translateSnapValues[TranslatecurrentSnapIndex];
		break;
	case ROTATE:
		operation = ImGuizmo::ROTATE;
		for (int i{}; i < 3; ++i)
			snapValue[i] = rotateSnapValues[RotatecurrentSnapIndex];
		break;
	case SCALE:
		operation = ImGuizmo::SCALE;
		for(int i{}; i<3; ++i)
			snapValue[i] = scaleSnapValues[ScalecurrentSnapIndex];
		break;
	}

	ImGuizmo::BeginFrame();
	XMFLOAT4X4 newObjectMatrix = {};
	if (m_pCamera) {
		XMFLOAT4X4 projectionTemp = m_pCamera->GetProjectionMatrix();
		XMMATRIX projectionMatrix = XMLoadFloat4x4(&projectionTemp);

		XMFLOAT4X4 viewTemp = m_pCamera->GetViewMatrix();
		XMMATRIX viewMatrix = XMLoadFloat4x4(&viewTemp);

		float viewArray[16];
		float projectionArray[16];
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(viewArray), viewMatrix);
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(projectionArray), projectionMatrix);
		if (m_pSelectedObject) {
			ImGuizmo::MODE mode = ImGuizmo::WORLD;

			XMFLOAT4X4 ObjectTemp = m_pSelectedObject->GetObjectMatrix();
			XMMATRIX ObjectMatrix = XMLoadFloat4x4(&ObjectTemp);
			float ObjectArray[16];
			XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(ObjectArray), ObjectMatrix);
		
			ImGuizmo::Manipulate(viewArray, projectionArray, operation, mode, ObjectArray, nullptr, snapValue);
			XMStoreFloat4x4(&newObjectMatrix, XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(ObjectArray)));
			
		}
	}
	
	ImGui::Begin("Editor");
	{
		ImGui::SetWindowPos(ImVec2(m_nWndClientWidth - 260, 0));
		ImGui::SetWindowSize(ImVec2(260, 340));
		if (ImGui::CollapsingHeader("Transform"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			if (m_pSelectedObject)
			{
				XMFLOAT4X4 objectMatrix = m_pSelectedObject->GetObjectMatrix();
				XMVECTOR scale, rotationQuat, translation;
				XMMatrixDecompose(&scale, &rotationQuat, &translation, XMLoadFloat4x4(&objectMatrix));

				if (ImGuizmo::IsUsing())
				{
					m_pSelectedObject->SetObjectMatrix(newObjectMatrix);
					QuaternionToEuler(rotationQuat, roty, rotz, rotx);
				}

				// Position
				XMFLOAT3 position = m_pSelectedObject->GetPosition();
				ImGui::Text("Position");
				ImGui::PushItemWidth(60);
				ImGui::Text("X"); ImGui::SameLine();
				ImGui::InputFloat("##X", &position.x);
				ImGui::SameLine();
				ImGui::Text("Y"); ImGui::SameLine();
				ImGui::InputFloat("##Y", &position.y);
				ImGui::SameLine();
				ImGui::Text("Z"); ImGui::SameLine();
				ImGui::InputFloat("##Z", &position.z);
				ImGui::PopItemWidth();
				m_pSelectedObject->SetPosition(position.x, position.y, position.z);

				// Rotation
				ImGui::Text("Rotation");
				ImGui::PushItemWidth(60);
				ImGui::Text("X"); ImGui::SameLine();
				bool changedX = ImGui::InputFloat("##rX", &rotx); ImGui::SameLine();
				ImGui::Text("Y"); ImGui::SameLine();
				bool changedY = ImGui::InputFloat("##rY", &roty);  ImGui::SameLine();
				ImGui::Text("Z"); ImGui::SameLine();
				bool changedZ = ImGui::InputFloat("##rZ", &rotz); 
				ImGui::PopItemWidth();

				if (changedX || changedY || changedZ)
				{
					ClampEulerAngles(rotx);
					ClampEulerAngles(roty);
					ClampEulerAngles(rotz);
					rotationQuat = EulerToQuaternion(roty, rotz, rotx);
					XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
					XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);
					XMMATRIX translationMatrix = XMMatrixTranslationFromVector(translation);
					XMMATRIX combinedMatrix = scaleMatrix * rotationMatrix * translationMatrix;
					XMStoreFloat4x4(&newObjectMatrix, combinedMatrix);
					m_pSelectedObject->SetObjectMatrix(newObjectMatrix);
				}

				// Scale
				ImGui::Text("Scale");
				ImGui::PushItemWidth(60);
				float scaleX = XMVectorGetX(scale);
				float scaleY = XMVectorGetY(scale);
				float scaleZ = XMVectorGetZ(scale);
				ImGui::Text("X"); ImGui::SameLine();
				bool changedScaleX = ImGui::InputFloat("##sX", &scaleX);
				ImGui::SameLine();
				ImGui::Text("Y"); ImGui::SameLine();
				bool changedScaleY = ImGui::InputFloat("##sY", &scaleY);
				ImGui::SameLine();
				ImGui::Text("Z"); ImGui::SameLine();
				bool changedScaleZ = ImGui::InputFloat("##sZ", &scaleZ);
				ImGui::PopItemWidth();

				if (changedScaleX || changedScaleY || changedScaleZ)
				{
					scale = XMVectorSet(scaleX, scaleY, scaleZ, 1.0f); 
					XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);
					XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);
					XMMATRIX translationMatrix = XMMatrixTranslationFromVector(translation);
					XMMATRIX combinedMatrix = scaleMatrix * rotationMatrix * translationMatrix;
					XMStoreFloat4x4(&newObjectMatrix, combinedMatrix);
					m_pSelectedObject->SetObjectMatrix(newObjectMatrix);
				}
			}
			else
			{
				// Display default values if no object is selected
				ImGui::Text("Position");
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::PushItemWidth(60);
				ImGui::Text("X: 0.0   Y: 0.0   Z: 0.0");
				ImGui::PopItemWidth();
				ImGui::PopStyleVar();

				ImGui::Text("Rotation");
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::PushItemWidth(60);
				ImGui::Text("X: 0.0   Y: 0.0   Z: 0.0");
				ImGui::PopItemWidth();
				ImGui::PopStyleVar();

				ImGui::Text("Scale");
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::PushItemWidth(60);
				ImGui::Text("X: 0.0   Y: 0.0   Z: 0.0");
				ImGui::PopItemWidth();
				ImGui::PopStyleVar();
			}
		}

		if (ImGui::CollapsingHeader("Object"))
		{
			if (ImGui::Button("Cube", ImVec2(120, 50))) {
				waitForGpuComplete();
				m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
				CMesh* clone = new CMesh();
				clone->CloneMesh(*importObj[3], m_pd3dDevice, m_pd3dCommandList);
				if (m_pScene) m_pScene->BuildObj(m_pd3dDevice, m_pd3dCommandList, "box", clone, m_TextureSRV);

				m_pd3dCommandList->Close();

				ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
				m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
				waitForGpuComplete();
				if (m_pScene) m_pScene->ReleaseUploadBuffers();

			}
			ImGui::SameLine();
			if (ImGui::Button("Wall", ImVec2(120, 50)))
			{
				waitForGpuComplete();
				m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
				CMesh* clone = new CMesh();
				clone->CloneMesh(*importObj[0], m_pd3dDevice, m_pd3dCommandList);
				if (m_pScene) m_pScene->BuildObj(m_pd3dDevice, m_pd3dCommandList, "wall", clone, m_TextureSRV);

				m_pd3dCommandList->Close();

				ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
				m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
				waitForGpuComplete();
				if (m_pScene) m_pScene->ReleaseUploadBuffers();

			}
			if (ImGui::Button("Chest", ImVec2(120, 50)))
			{
				waitForGpuComplete();
				m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
				CMesh* clone = new CMesh();
				clone->CloneMesh(*importObj[1], m_pd3dDevice, m_pd3dCommandList);
				if (m_pScene) m_pScene->BuildObj(m_pd3dDevice, m_pd3dCommandList, "chest", clone, m_TextureSRV);

				m_pd3dCommandList->Close();

				ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
				m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
				waitForGpuComplete();
				if (m_pScene) m_pScene->ReleaseUploadBuffers();

			}
			ImGui::SameLine();
			if (ImGui::Button("Flat", ImVec2(120, 50))) {
				waitForGpuComplete();
				m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
				CMesh* clone = new CMesh();
				clone->CloneMesh(*importObj[2], m_pd3dDevice, m_pd3dCommandList);
				if (m_pScene) m_pScene->BuildObj(m_pd3dDevice, m_pd3dCommandList, "grass", clone, m_TextureSRV);

				m_pd3dCommandList->Close();

				ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
				m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
				waitForGpuComplete();
				if (m_pScene) m_pScene->ReleaseUploadBuffers();

			}
		}
	}
	
	ImGui::End();

	ImGui::Begin("ctrl"); 
	{
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(200, 55));
		if (ImGui::Button("Translate")) {
			currentTransformMode = TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Rotate")) {
			currentTransformMode = ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Scale")) {
			currentTransformMode = SCALE;
		}
	}
	ImGui::End();

	ImGui::Begin("Snapping");
	{
		
		ImGui::SetWindowSize(ImVec2(170, 140));

		if (ImGui::Button("Translate", ImVec2(100, 30)))
		{
			ImGui::OpenPopup("TranslateSnapSelectorPopup");
		}
		if (ImGui::BeginPopup("TranslateSnapSelectorPopup")) {
			for (int i = 0; i < sizeof(translateSnapValues) / sizeof(float); ++i) {
				char label[32];
				snprintf(label, sizeof(label), "%.2f", translateSnapValues[i]);

				if (ImGui::RadioButton(label, &TranslatecurrentSnapIndex, i)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text("%.2f", translateSnapValues[TranslatecurrentSnapIndex]);
		
		
		
		if (ImGui::Button("Rotate", ImVec2(100, 30))) {
			ImGui::OpenPopup("RotateSnapSelectorPopup");
		}
		if (ImGui::BeginPopup("RotateSnapSelectorPopup")) {

			for (int i = 0; i < sizeof(rotateSnapValues) / sizeof(float); ++i) {
				char label[32];
				snprintf(label, sizeof(label), "%.2f", rotateSnapValues[i]);

				if (ImGui::RadioButton(label, &RotatecurrentSnapIndex, i)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text("%.2f", rotateSnapValues[RotatecurrentSnapIndex]);

		
		if (ImGui::Button("Scale", ImVec2(100, 30))) {
			ImGui::OpenPopup("ScaleSnapSelectorPopup");
		}
		if (ImGui::BeginPopup("ScaleSnapSelectorPopup")) {

			for (int i = 0; i < sizeof(scaleSnapValues) / sizeof(float); ++i) {
				char label[32];
				snprintf(label, sizeof(label), "%.2f", scaleSnapValues[i]);

				if (ImGui::RadioButton(label, &ScalecurrentSnapIndex, i)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text("%.2f", scaleSnapValues[ScalecurrentSnapIndex]);
	}
	ImGui::End();
	ImGui::Render();
	

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	
	//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다. 
#ifdef _WITH_PLAYER_TOP
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다. 
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	

	m_pd3dCommandList->SetDescriptorHeaps(1, &m_TextureSRV);
	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);
	if (m_pGrid) m_pGrid->Render(m_pd3dCommandList, m_pCamera);
	

	m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dSrvDescriptorImGUIHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pd3dCommandList);
	
	
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	hResult = m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	
	waitForGpuComplete();
	m_pdxgiSwapChain->Present(0, 0);
	MoveToNextFrame();
	
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);

}

void CGameFramework::waitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue){
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}

}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	
	default:
		break;
	}
}
void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID){
	case WM_KEYUP:
		switch (wParam){
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
			//“F9” 키가 눌려지면 윈도우 모드와 전체화면 모드의 전환을 처리한다. 
		case VK_F9:
			ChangeSwapChainState();
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1),
				m_GameTimer.GetTimeElapsed());
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		if(wParam =='W' or wParam == 'w')
			currentTransformMode = TRANSLATE;
		if (wParam == 'R' or wParam == 'r')
			currentTransformMode = SCALE;
		if (wParam == 'E' or wParam == 'e')
			currentTransformMode = ROTATE;		
		if (wParam == VK_DELETE) {
			if (m_pSelectedObject){
				if (m_pScene) m_pScene->RemoveObject(m_pSelectedObject);
				m_pSelectedObject = NULL;
			
			}
		}
		if (wParam == VK_END) {
			if (m_pSelectedObject) {
				if (m_pScene) m_pScene->SnapToFloor(*m_pSelectedObject);

			}
		}
		if (wParam == 'C' && GetAsyncKeyState(VK_CONTROL))
		{
			if (m_pSelectedObject) {
				m_pCopiedObject = m_pSelectedObject;
			}
		}
		if (wParam == 'V' && GetAsyncKeyState(VK_CONTROL))
		{
			if (m_pCopiedObject and m_pScene)
			{
				waitForGpuComplete();
				Sleep(100);
				m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
				m_pScene->AddCopiedObject(m_pd3dDevice, m_pd3dCommandList,m_pCopiedObject, m_TextureSRV);

				waitForGpuComplete();
				m_pd3dCommandList->Close();
				ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
				m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

				waitForGpuComplete();
				if (m_pScene) m_pScene->ReleaseUploadBuffers();
			}
		}
		break;
	default:
		break;
	}
}
LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse) {
		switch (nMessageID) {
		case WM_SIZE:
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);
			break;
	
		case WM_LBUTTONDOWN:
			m_pSelectedObject = m_pScene->PickObjectPointedByCursor(LOWORD(lParam),
				HIWORD(lParam), m_pCamera);
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_RBUTTONDOWN:
			m_pSelectedObject = NULL;
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		}
	}
	else{
		switch (nMessageID)
		{
		case WM_KEYDOWN:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		}
	}
		return (0);
}

void CGameFramework::ChangeSwapChainState()
{
	waitForGpuComplete();
	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);
	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);
	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	CreateRenderTargetViews();
}
void CGameFramework::MoveToNextFrame() 
{
		m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
		UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
		HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
		if (m_pd3dFence->GetCompletedValue() < nFenceValue)
		{
			hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
			::WaitForSingleObject(m_hFenceEvent, INFINITE);
		}
}
void CGameFramework::ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta)
{
	//픽킹으로 선택한 게임 객체가 있으면 키보드를 누르거나 마우스를 움직이면 게임 개체를 이동 또는 회전한다. 
	if (dwDirection != 0)
	{
		/*if (dwDirection & DIR_FORWARD) m_pSelectedObject->MoveForward(+1.0f);
		if (dwDirection & DIR_BACKWARD) m_pSelectedObject->MoveForward(-1.0f);
		if (dwDirection & DIR_LEFT) m_pSelectedObject->MoveStrafe(+1.0f);
		if (dwDirection & DIR_RIGHT) m_pSelectedObject->MoveStrafe(-1.0f);
		if (dwDirection & DIR_UP) m_pSelectedObject->MoveUp(+1.0f);
		if (dwDirection & DIR_DOWN) m_pSelectedObject->MoveUp(-1.0f);*/
	}
	
}
void CGameFramework::SaveObject(std::ofstream& outFile)
{
	if (m_pScene) m_pScene->SaveFile(outFile);
}
void CGameFramework::ReadObject(std::ifstream& inFile)
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	if (m_pScene) m_pScene->ReadFile(m_pd3dDevice, m_pd3dCommandList,inFile, m_TextureSRV);
	waitForGpuComplete();
	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	waitForGpuComplete();
	if (m_pScene) m_pScene->ReleaseUploadBuffers();
}

void CGameFramework::ResetScene()
{
	if (m_pScene) m_pScene->ResetScene();
}
void CGameFramework::CreateShaderVariables()
{
	if (m_pScene) m_pScene->CreateShaderVariablesToMesh(m_pd3dDevice, m_pd3dCommandList);
}
