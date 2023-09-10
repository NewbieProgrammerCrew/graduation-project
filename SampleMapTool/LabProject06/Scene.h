#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Camera.h"
#include "Player.h"
#include "stdafx.h"
struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT3 m_xmf3Position;
	float m_fFalloff;
	XMFLOAT3 m_xmf3Direction;;
	float m_fTheta; //cos(m_fTheta)
	XMFLOAT3 m_xmf3Attenuation;
	float m_fPhi; //cos(m_fPhi)
	bool m_bEnable;
	int m_nType;
	float m_fRange;
	float padding;
};
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
};
struct MATERIALS
{
	MATERIAL m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
	//���� ��� ����� ������ ����
	void BuildLightsAndMaterials();
	//���� ��� ����� ������ ���� ���ҽ��� �����ϰ� ����
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	 void CreateShaderVariablesToMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
public:
	CPlayer* m_pPlayer = nullptr;
protected:
	//���� ����
	LIGHTS* m_pLights = nullptr;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�. 
	ComPtr<ID3D12Resource> m_pd3dcbLights;
	LIGHTS* m_pcbMappedLights = nullptr;
	//���� ��ü�鿡 ����Ǵ� ����
	MATERIALS* m_pMaterials = nullptr;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�. 
	ComPtr<ID3D12Resource> m_pd3dcbMaterials;
	MATERIAL* m_pcbMappedMaterials =nullptr;
	std::shared_ptr<CHeightMapTerrain> m_pTerrain = nullptr;
	CGameObject* m_pSkyAtmosphere = nullptr;
public:
	CScene();
	~CScene() {};
	std::shared_ptr <CGameObject> PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
	void GenerateHeightMap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LPCTSTR path);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildSky(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CMesh* sObjMesh, ID3D12DescriptorHeap* m_pSRVHeap);
	void BuildObj(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& name, CMesh* m_pSelectedObj, ID3D12DescriptorHeap* m_pSRVHeap);
	void SaveFile(std::ofstream& outFile);
	void ExportMap(std::ofstream& outFile);
	std::shared_ptr <CHeightMapTerrain> GetSharedTerrain() { return m_pTerrain; }
	CHeightMapTerrain* GetTerrain() { return(m_pTerrain.get()); }

	void ReadFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::ifstream& inFile, ID3D12DescriptorHeap* m_pSRVHeap);
	void ResetScene();
	void RemoveObject(std::shared_ptr <CGameObject> rObject);
	void ProcessDeletionQueue();
	void AddCopiedObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::shared_ptr <CGameObject> cpyObject , ID3D12DescriptorHeap* m_pSRVHeap);
	void ReleaseObjects(); 
	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = nullptr;
	ID3D12PipelineState *m_pd3dPipelineState = nullptr;
	void ReleaseUploadBuffers();
	ID3D12RootSignature* GetGraphicsRootSignature();
	void SnapToFloor(CGameObject& target);

protected:
	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	CObjectsShader *m_pShaders = nullptr;
	int m_nShaders = 0;
};

