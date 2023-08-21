#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"
#include "Grid.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"
#include "Mesh.h"


class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	ImGuizmo::OPERATION operation;
	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory;			//DXGI 팩토리 인터페이스에 대한 포인터
	IDXGISwapChain3* m_pdxgiSwapChain;		//스왑 체인 인터페이스에 대한 포인터 (주로 디스플레이를 제어하기 위해 필요)
	ID3D12Device* m_pd3dDevice;				//Direct3D 디바이스 인터페이스에 대한 포인터 (주로 리소스를 생성하기 위해 필요)

	ID3D12Resource* m_pd3dSrvBuffer;

	ID3D12DescriptorHeap* m_TextureSRV;
	UINT m_nSrvDescriptorIncrementSize;
	UINT m_nTextureSRVSize;

	ID3D12DescriptorHeap* m_pd3dSrvDescriptorImGUIHeap;

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;			// MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다 (안티 앨리어싱)
	static const UINT m_nSwapChainBuffers = 2;  // 스왑 체인의 후면 버퍼의 개수이다.
	UINT m_nSwapChainBufferIndex;				// 현재 스왑 체인의 후면 버퍼 인덱스

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기


	ID3D12Resource* m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	//깊이 - 스텐실 버퍼. 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기

	ID3D12CommandQueue* m_pd3dCommandQueue;
	ID3D12CommandAllocator* m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
	//명령 큐, 명령 할당자, 명령리스트 인터페이스 포인터

	ID3D12PipelineState* m_pd3dPipelineState;
	//그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터이다.

	ID3D12Fence* m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;
	//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.

	//다음은 게임 프레임워크에서 사용할 타이머이다. 
	CGameTimer m_GameTimer;
	//다음은 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.
	_TCHAR m_pszFrameRate[50];

	CScene* m_pScene;
	std::shared_ptr <CGameObject> m_pSelectedObject = NULL;
	std::shared_ptr <CGameObject> m_pCopiedObject = NULL;
	std::deque<OBJMesh*> importObj;

	enum TransformMode
	{
		TRANSLATE,
		ROTATE,
		SCALE
	};
	TransformMode currentTransformMode;
	bool snapping = false;
	
	float translateSnapValues[5] = {0.0f, 1.0f, 5.0f, 10.0f, 50.0f };
	float rotateSnapValues[8] = { 0.0f,5.0f,10.0f, 15.0f,30.0f, 45.0f,60.0f, 90.0f };
	float scaleSnapValues[7] = { 0.0f, 1.0f,0.5f,0.25f, 0.125f, 0.0625f, 0.03125f };

	float snapValue[3] = { 1.0f,1.0f,1.0f };
	int TranslatecurrentSnapIndex = 3;
	int RotatecurrentSnapIndex = 1;
	int ScalecurrentSnapIndex = 3;
	float rotx = 0;
	float roty = 0;
	float rotz = 0;

	const float identityMatrix[16] =
	{ 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };

public:
	CPlayer* m_pPlayer = NULL;
	CCamera* m_pCamera = NULL;
	CGrid* m_pGrid = NULL;
	POINT m_ptOldCursorPos;
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);//프레임워크를 초기화하는 함수 ( 주 윈도우가 생성되면 호출)
	void OnDestroy();
	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	void CreateSwapChain();					//스왑체인
	void CreateRtvAndDsvDescriptorHeaps();	
	void CreateDirect3DDevice();			//서술자힙
	void CreateShaderResourceView();
	void CreateCommandQueueAndList();		//명령 큐/할당자/리스트
	//생성하는 함수

	void CreateRenderTargetViews(); //렌더 타겟 뷰
	void CreateDepthStencilView(); //깊이 - 스텐실 뷰
	//생성하는 함수

	void BuildObjects();
	void ReleaseObjects();
	void ResetScene();
	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수
	void CreateShaderVariables();
	///////프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수///////
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void waitForGpuComplete(); //CPU와 GPU를 동기화하는 함수

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	void ChangeSwapChainState();
	void MoveToNextFrame();
	void SaveObject(std::ofstream& outFile);
	void ReadObject(std::ifstream& inFile);
};

