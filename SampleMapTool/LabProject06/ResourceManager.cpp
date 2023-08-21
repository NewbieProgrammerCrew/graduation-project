#include "ResourceManager.h"
#include "DDSTextureLoader.h"
CResourceManager::CResourceManager() {

    

}
CResourceManager::~CResourceManager()
{
}

// 텍스처 로드 함수
void CResourceManager::LoadTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std:: string& objName, const std::string& path, int offset)
{
    auto Tex = std::make_unique<Texture>();
    Tex->Name = objName;
    std::wstring wstr(path.begin(), path.end());
    Tex->Filename = wstr;
    Tex->offset = offset;
    DirectX::CreateDDSTextureFromFile12(pd3dDevice, pd3dCommandList, Tex->Filename.c_str(), Tex->Resource, Tex->UploadHeap);
    m_Textures[Tex->Name]=std::move(Tex);
}

// 텍스처 가져오기 함수
ComPtr <ID3D12Resource> CResourceManager::GetTexture(const std::string& name)
{
    if (m_Textures.find(name) == m_Textures.end()) {
       return m_Textures["default"]->Resource;
    }
    return m_Textures[name]->Resource;
}
int CResourceManager::GetOffset(const std::string& name)
{
    if (m_Textures.find(name) == m_Textures.end()) {
        return -1;
    }
    return m_Textures[name]->offset;
}
// 리소스 해제 함수
void CResourceManager::ReleaseResources()
{

}