#pragma once
//정점을 표현하기 위한 클래스를 선언한다. 
#include "stdafx.h"
#include <algorithm>
class CVertex
{
protected:
	//정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다). 
	XMFLOAT3 m_xmf3Position;
public: 
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
protected:
	//정점의 색상이다. 
	XMFLOAT4 m_xmf4Diffuse;
public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = xmf3Position;
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

class CIlluminatedVertex : public CVertex
{
protected:
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT2 m_xmf2Texture;
public:
	CIlluminatedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2Texture = XMFLOAT2(0.f, 0.f)) {
		m_xmf3Position = XMFLOAT3(x, y, z); m_xmf3Normal = xmf3Normal;  m_xmf2Texture = xmf2Texture;
	}
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2Texture = XMFLOAT2(0.f, 0.f)) {
		m_xmf3Position = xmf3Position; m_xmf3Normal = xmf3Normal;  m_xmf2Texture = xmf2Texture;
	}
	~CIlluminatedVertex() { }
};
class CMesh
{
public:
	CMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	CMesh() {}
	virtual ~CMesh();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
	int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float * pfNearHitDistance);
	BoundingOrientedBox GetBoundingBox() { return(m_xmBoundingBox); }

	BoundingOrientedBox m_xmBoundingBox;

protected:
	ComPtr <ID3D12Resource> m_pd3dVertexBuffer;
	ComPtr <ID3D12Resource> m_pd3dVertexUploadBuffer;

	ComPtr <ID3D12Resource> m_pd3dIndexBuffer;
	ComPtr <ID3D12Resource> m_pd3dIndexUploadBuffer;

	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	UINT m_nIndices = 0;
	UINT m_nStartIndex = 0;
	int m_nBaseVertex = 0;

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
	XMFLOAT3 BoundingBoxExtents = XMFLOAT3(0, 0, 0);
	XMFLOAT3 BoundingBoxCenter = XMFLOAT3(0, 0, 0);


	//CDiffusedVertex* m_Vertices = NULL;
	std::vector<CDiffusedVertex> m_Vertices;
	std::vector<CIlluminatedVertex> m_IVertices;


	//UINT* m_Indices = NULL;
	std::vector<UINT> m_Indices;
public:
	void CloneMesh(const CMesh& other, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
		m_nReferences = other.m_nReferences;
		m_xmBoundingBox = other.m_xmBoundingBox;
		m_nIndices = other.m_nIndices;
		m_nStartIndex = other.m_nStartIndex;
		m_nBaseVertex = other.m_nBaseVertex;
		m_d3dVertexBufferView = other.m_d3dVertexBufferView;
		m_d3dPrimitiveTopology = other.m_d3dPrimitiveTopology;
		m_nSlot = other.m_nSlot;
		m_nVertices = other.m_nVertices;
		m_nStride = other.m_nStride;
		m_nOffset = other.m_nOffset;
		BoundingBoxExtents = other.BoundingBoxExtents;
		BoundingBoxCenter = other.BoundingBoxCenter;
		m_Vertices = other.m_Vertices;
		m_IVertices = other.m_IVertices;
		m_Indices = other.m_Indices;

		m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_IVertices.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
		m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
		m_d3dVertexBufferView.StrideInBytes = m_nStride;
		m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

		m_Indices.resize(m_nIndices);

		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Indices.data(), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
		m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
		m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

		m_xmBoundingBox = BoundingOrientedBox(BoundingBoxCenter, BoundingBoxExtents, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void writeDataToFile(std::ostream& outFile)
	{
		// 배열 크기를 먼저 씁니다.
		outFile.write(reinterpret_cast<const char*>(&m_nVertices), sizeof(m_nVertices));
		outFile.write(reinterpret_cast<const char*>(m_IVertices.data()), m_nVertices * sizeof(CIlluminatedVertex));

		outFile.write(reinterpret_cast<const char*>(&m_nIndices), sizeof(m_nIndices));
		outFile.write(reinterpret_cast<const char*>(m_Indices.data()), m_nIndices * sizeof(UINT));
		outFile.write(reinterpret_cast<const char*>(&BoundingBoxCenter), sizeof(BoundingBoxCenter));
		outFile.write(reinterpret_cast<const char*>(&BoundingBoxExtents), sizeof(BoundingBoxExtents));
	}
	virtual void readDataToFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, std::ifstream& inFile)
	{

		inFile.read(reinterpret_cast<char*>(&m_nVertices), sizeof(m_nVertices));

		m_nStride = sizeof(CIlluminatedVertex);
		m_IVertices.resize(m_nVertices);
		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		inFile.read(reinterpret_cast<char*>(m_IVertices.data()), m_nVertices * sizeof(CIlluminatedVertex));
		/*std::string message = "count Vertice: " + std::to_string(m_IVertices.empty()) + "\n";
		OutputDebugStringA(message.c_str());*/
		if (!m_IVertices.empty()) {
			m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_IVertices.data(), m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
			m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
			m_d3dVertexBufferView.StrideInBytes = m_nStride;
			m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
		}
		inFile.read(reinterpret_cast<char*>(&m_nIndices), sizeof(m_nIndices));
		m_Indices.resize(m_nIndices);
		inFile.read(reinterpret_cast<char*>(m_Indices.data()), m_nIndices * sizeof(UINT));
		if (!m_Indices.empty()) {

			m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_Indices.data(), sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
			m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
			m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

			inFile.read(reinterpret_cast<char*>(&BoundingBoxCenter), sizeof(BoundingBoxCenter));
			inFile.read(reinterpret_cast<char*>(&BoundingBoxExtents), sizeof(BoundingBoxExtents));

			m_xmBoundingBox = BoundingOrientedBox(BoundingBoxCenter, BoundingBoxExtents, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


		}
	}

};
class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CTriangleMesh() { }
};
class CCubeMeshDiffused : public CMesh
{
public:
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다. 
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
}; 
class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};
class CSphereMeshDiffused : public CMesh
{
public:
	CSphereMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshDiffused();
};

class CGridMesh : public CMesh
{
public:
	CGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, float fWidth = 100.0f, float fHeight = 100.0f, float fDepth = 100.0f);
	virtual ~CGridMesh();
};
class CMeshIlluminated : public CMesh
{
public:

	CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual ~CMeshIlluminated();
public:
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices);
	
};
class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};
struct VertexKey {
	int positionIndex;
	int uvIndex;
	int normalIndex;

	bool operator<(const VertexKey& other) const {
		if (positionIndex < other.positionIndex) return true;
		if (positionIndex > other.positionIndex) return false;

		if (uvIndex < other.uvIndex) return true;
		if (uvIndex > other.uvIndex) return false;

		return normalIndex < other.normalIndex;
	}
};
class OBJMesh : public CMeshIlluminated
{
private:
		std::vector<XMFLOAT3> vertexPositions;
		std::vector<uint32_t> indices;
		std::map<VertexKey, uint32_t> uniqueVertices;
public:
	OBJMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& filepath);
	virtual ~OBJMesh() {}

};

class SkyAtmosphere : public CMeshIlluminated
{
private:
	std::vector<XMFLOAT3> vertexPositions;
	std::vector<uint32_t> indices;
	std::map<VertexKey, uint32_t> uniqueVertices;
public:
	SkyAtmosphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& filepath);
	virtual ~SkyAtmosphere() {}
};


class CHeightMapImage
{
private:
	BYTE *m_pHeightMapPixels;
	int m_nWidth;
	int m_nLength;
	XMFLOAT3 m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	float GetHeight(float x, float z);

	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};


class CHeightMapGridMesh : public CMesh
{
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)이다. 
	int m_nWidth;
	int m_nLength;
	
	/*격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터이다. 
	실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 갖는다.
	즉, 실제 격자의 x-축 방향의 간격은 1이 아니라 스케일 벡터의 x-좌표가 된다. 이렇게 하면 작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할
	수 있다.*/
	
	XMFLOAT3 m_xmf3Scale;
public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList	* pd3dCommandList, 
		int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), 
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void * pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }
	
	virtual float OnGetHeight(int x, int z, void *pContext);
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};