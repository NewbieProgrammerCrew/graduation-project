#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;
struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
};
class CMaterial
{
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	//재질의 기본 색상
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//재질의 번호
	UINT m_nReflection = 0;
	//재질을 적용하여 렌더링을 하기 위한 쉐이더
	CShader* m_pShader = nullptr;
	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(CShader* pShader);

public:
	CMaterial();
	virtual ~CMaterial();
	CMaterial(const CMaterial& other)
	{
		m_xmf4Albedo = other.m_xmf4Albedo;
		m_nReflection = other.m_nReflection;

		
		m_pShader = other.m_pShader;
	
		m_nReferences = 0;
	}
	void writeDataToFile(std::ofstream& outFile) {
		outFile.write(reinterpret_cast<const char*>(&m_nReflection), sizeof(m_nReflection));
	}

private:
	int m_nReferences = 0;
	
};

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:

	int m_nReferences = 0;
	int index;

public:
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	CMaterial* m_pMaterial = nullptr;
	XMFLOAT4X4 m_xmf4x4World;

protected:
	ID3D12DescriptorHeap* m_pTextureSRVHeap;
	CMesh* m_pMesh = nullptr;
	std::string m_name;

public:
	bool IsVisible(CCamera* pCamera = nullptr);

	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
	void SetMaterial(CMaterial* pMaterial);
	void SetMaterial(UINT nReflection);

	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh* pMesh);

	virtual void SetShader(CShader* pShader);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void SetRotate(float fPitch,float fYaw, float fRoll);

	CMesh* GetMesh() { if (m_pMesh) return m_pMesh; else return nullptr; }
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	std::string GetName() { return m_name; }
	ID3D12DescriptorHeap* GetSrvHeap() { return m_pTextureSRVHeap; }
	XMFLOAT4X4 GetObjectMatrix() { return m_xmf4x4World; }
	int GetIndex() { return index; }
	void SetName(const std::string& name) { m_name = name; }
	void SetIndex(int in) { index = in; }
	void SetObjectMatrix(XMFLOAT4X4 newObjectMatrix);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void SetSRVHeap(ID3D12DescriptorHeap* pSRVHeap);
	virtual void writeToFile(std::ofstream& outFile)
	{
		outFile.write(reinterpret_cast<const char*>(&m_xmf4x4World), sizeof(m_xmf4x4World));
		outFile.write(m_name.c_str(), 100);
		if(m_pMesh)
			m_pMesh->writeDataToFile(outFile);
	}
	virtual void readToFile(std::ifstream& inFile)
	{
		inFile.read(reinterpret_cast<char*>(&m_xmf4x4World), sizeof(m_xmf4x4World));
		
		std::vector<char> buffer(100);
		inFile.read(buffer.data(), 100);

		m_name.assign(buffer.data(), 100);
		m_name.erase(std::find(m_name.begin(), m_name.end(), '\0'), m_name.end());
		
		
	}

	virtual void ExportToFile(std::ofstream& outFile)
	{
		int len = m_name.size() + 1;
		
		outFile.write(reinterpret_cast<const char*>(&len), sizeof(int));

		std::wstring ws = std::to_wstring(len)+L"\n";
		OutputDebugString(ws.c_str());

		outFile.write(m_name.c_str(), len);
		XMFLOAT3 position = { m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43 };
	
		outFile.write(reinterpret_cast<const char*>(&position.x), sizeof(float));
		outFile.write(reinterpret_cast<const char*>(&position.y), sizeof(float));
		outFile.write(reinterpret_cast<const char*>(&position.z), sizeof(float));
		ws = std::to_wstring(position.x) + L"\n";
		OutputDebugString(ws.c_str());

		XMMATRIX matrix = XMLoadFloat4x4(&m_xmf4x4World);
		XMVECTOR quaternion = XMQuaternionRotationMatrix(matrix);

		float q0 = XMVectorGetW(quaternion);
		float q1 = XMVectorGetX(quaternion);
		float q2 = XMVectorGetY(quaternion);
		float q3 = XMVectorGetZ(quaternion);

		float roll = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3));
		float yaw = asinf(2.0f * (q0 * q2 - q3 * q1));
		float pitch = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2));

		// Convert to degrees
		const float RAD_TO_DEG = (180.0f / 3.14159265f);
		pitch *= RAD_TO_DEG;
		yaw *= RAD_TO_DEG;
		roll *= RAD_TO_DEG;

		outFile.write(reinterpret_cast<const char*>(&pitch), sizeof(float));
		outFile.write(reinterpret_cast<const char*>(&yaw), sizeof(float));
		outFile.write(reinterpret_cast<const char*>(&roll), sizeof(float));
	
	
		XMFLOAT3 scale;
		scale.x = sqrt(m_xmf4x4World._11 * m_xmf4x4World._11 + m_xmf4x4World._12 * m_xmf4x4World._12 + m_xmf4x4World._13 * m_xmf4x4World._13);
		scale.y = sqrt(m_xmf4x4World._21 * m_xmf4x4World._21 + m_xmf4x4World._22 * m_xmf4x4World._22 + m_xmf4x4World._23 * m_xmf4x4World._23);
		scale.z = sqrt(m_xmf4x4World._31 * m_xmf4x4World._31 + m_xmf4x4World._32 * m_xmf4x4World._32 + m_xmf4x4World._33 * m_xmf4x4World._33);


		outFile.write(reinterpret_cast<const char*>(&scale.x), sizeof(float));
		outFile.write(reinterpret_cast<const char*>(&scale.y), sizeof(float));
		outFile.write(reinterpret_cast<const char*>(&scale.z), sizeof(float));
	}
};

class CCubeObject : public CGameObject
{
public:
	CCubeObject();
	virtual ~CCubeObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed) {};
}; 
