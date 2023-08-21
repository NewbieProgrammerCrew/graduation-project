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

	IDXGIFactory4* m_pdxgiFactory;			//DXGI ���丮 �������̽��� ���� ������
	IDXGISwapChain3* m_pdxgiSwapChain;		//���� ü�� �������̽��� ���� ������ (�ַ� ���÷��̸� �����ϱ� ���� �ʿ�)
	ID3D12Device* m_pd3dDevice;				//Direct3D ����̽� �������̽��� ���� ������ (�ַ� ���ҽ��� �����ϱ� ���� �ʿ�)

	ID3D12Resource* m_pd3dSrvBuffer;

	ID3D12DescriptorHeap* m_TextureSRV;
	UINT m_nSrvDescriptorIncrementSize;
	UINT m_nTextureSRVSize;

	ID3D12DescriptorHeap* m_pd3dSrvDescriptorImGUIHeap;

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;			// MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ� (��Ƽ �ٸ����)
	static const UINT m_nSwapChainBuffers = 2;  // ���� ü���� �ĸ� ������ �����̴�.
	UINT m_nSwapChainBufferIndex;				// ���� ���� ü���� �ĸ� ���� �ε���

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	//���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ��


	ID3D12Resource* m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	//���� - ���ٽ� ����. ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ��

	ID3D12CommandQueue* m_pd3dCommandQueue;
	ID3D12CommandAllocator* m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
	//��� ť, ��� �Ҵ���, ��ɸ���Ʈ �������̽� ������

	ID3D12PipelineState* m_pd3dPipelineState;
	//�׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� �������̴�.

	ID3D12Fence* m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;
	//�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.

	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�. 
	CGameTimer m_GameTimer;
	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
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

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);//�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ� ( �� �����찡 �����Ǹ� ȣ��)
	void OnDestroy();
	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	void CreateSwapChain();					//����ü��
	void CreateRtvAndDsvDescriptorHeaps();	
	void CreateDirect3DDevice();			//��������
	void CreateShaderResourceView();
	void CreateCommandQueueAndList();		//��� ť/�Ҵ���/����Ʈ
	//�����ϴ� �Լ�

	void CreateRenderTargetViews(); //���� Ÿ�� ��
	void CreateDepthStencilView(); //���� - ���ٽ� ��
	//�����ϴ� �Լ�

	void BuildObjects();
	void ReleaseObjects();
	void ResetScene();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ�
	void CreateShaderVariables();
	///////�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ�///////
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void waitForGpuComplete(); //CPU�� GPU�� ����ȭ�ϴ� �Լ�

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	void ChangeSwapChainState();
	void MoveToNextFrame();
	void SaveObject(std::ofstream& outFile);
	void ReadObject(std::ifstream& inFile);
};

