#pragma once
#include "stdafx.h"


#define FIRST_PERSON_CAMERA 0x01
#define SPACESHIP_CAMERA	 0x02
#define THIRD_PERSON_CAMERA  0x03

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH)/float(FRAME_BUFFER_HEIGHT))

class CPlayer;

struct VS_CB_CAMERA_INFO {

	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3		m_xmf3Position;
};

class CCamera
{

protected:
	BoundingFrustum m_xmFrustum;
	//12장 추가
	//카메라의 위치 벡터이다.
	XMFLOAT3		m_xmf3Position;
	//카메라의 로컬 x축  y 축 z  축을 나타내는 벡터이다.
	XMFLOAT3		m_xmf3Right;
	XMFLOAT3		m_xmf3Up;
	XMFLOAT3		m_xmf3Look;

	//카메라 x y z축으로 얼마만큼 회전했는가를 나타내는 각도이다.
	float			m_fPitch;
	float			m_fRoll;
	float			m_fYaw;
	//카메라의 종류를 나타낸다.
	DWORD			m_nMode;

	//플레이어가 바라볼 위치벡터이다. 주로 3인칭 카메라에서 사용된다.
	XMFLOAT3		m_xmf3LookAtWorld;
	//플레이어와 카메라의 오프셋을 나타내는 벡터이다.
	XMFLOAT3		m_xmf3offset;
	//플레이어가 회전할때 얼만큼ㅁ의 시간을 지연시킨후 카메라를 회전시킬 것인가를 나타낸다.
	float			m_fTimeLag;

	XMFLOAT4X4		 m_xmf4x4View;
	XMFLOAT4X4		m_xmf4x4Projection;



	//카메라를 가지고 있는 플레이어에 대한 포인터이다.

	CPlayer* m_pPlayer = NULL;


	//뷰포트와 씨저 사각형
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	ID3D12Resource* m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO* m_pcbMappedCamera = NULL;

public:
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera() {};

	//카메라의 정보를 세이더 프로그램에게 전달하기 위한 상수버퍼를 생성하고 갱신한다.
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);


	//카메라 변환 행렬을 생성한다.
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Lookat, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	//투영 변환 행렬을 생성한다
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFovAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinz = 0.0f, float fMax = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	CPlayer* GetPlayer() { return (m_pPlayer); }

	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return (m_xmf3Position); }

	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }
	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return (m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return (m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3offset = xmf3Offset; }
	XMFLOAT3& GetOffset() { return(m_xmf3offset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }


	// 카메라를 xmf3Shift 만큼 이동한다.
	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }

	//카메라를 xyz축으로 회전하는 가상함수
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) {}
	//카메라의 이동, 회전에 따라 카메라의 정보를 갱신하는 가상함수이다.
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) {}
	//3인칭 카메라에서 카메라가 바라보는 지점을 설정한다. 일반적으로 플레이어를 바라보도록 설정한다.
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) {}

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);


	//절두체(월드 좌표계)를 생성한다. 
	void GenerateFrustum();
	//바운딩 박스(OOBB, 월드 좌표계)가 절두체에 포함되는 가를 검사한다. 
	bool IsInFrustum(BoundingOrientedBox& xmBoundingBox);
};

class CSpaceShipCamera : public CCamera
{
public:
	CSpaceShipCamera(CCamera* pCamera);
	virtual ~CSpaceShipCamera() { }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};


class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CCamera* pCamera);
	virtual ~CFirstPersonCamera() {}
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CCamera* pCamera);
	virtual ~CThirdPersonCamera() { }
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
};