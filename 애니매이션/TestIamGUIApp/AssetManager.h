#pragma once

#include <unordered_map>
#include <filesystem>
#include <wrl/client.h>
#include "AnimationClip.h"
#include "D2DRender.h"

// 예시입니다.
class AssetManager
{
public:
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;
   
    AssetManager() = default;
    ~AssetManager() = default;


    void LoadTexture(sample::D2DRenderer* d2dContext, const std::wstring& key, const std::filesystem::path& filePath);
    void LoadAseprite(sample::D2DRenderer* d2dContext, const std::wstring& key, const std::filesystem::path& filePath);
    AnimationClips GetClips(const std::wstring& key) const;


private:

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>> m_textures;
    std::unordered_map<std::wstring, AnimationClips> m_clipsMap;
};

