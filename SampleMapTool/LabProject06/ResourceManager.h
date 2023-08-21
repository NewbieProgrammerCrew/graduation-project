#pragma once
#include "stdafx.h"


struct Texture
{
    std::string Name;
    std::wstring Filename;
    int offset;
    ComPtr<ID3D12Resource> Resource = nullptr;
    ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

class CResourceManager
{
private:
    std::unordered_map <std::string, std::unique_ptr<Texture>> m_Textures;
    UINT SrvSize;
    CResourceManager();
    ~CResourceManager();



public:
    static CResourceManager& GetInstance()
    {
        static CResourceManager instance;
        return instance;
    }
    void LoadTexture(ID3D12Device* device, ID3D12GraphicsCommandList* pd3dCommandList, const std::string& objName, const std::string& path, int offset);
    ComPtr <ID3D12Resource> GetTexture(const std::string& path);
    int GetOffset(const std::string& name);
    void SetSrvSize(UINT size) { SrvSize = size; }
    UINT GetSrvSize() { return SrvSize; }
    void ReleaseResources();
};
