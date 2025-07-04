#include "pch.h"
#include "AnimationClip.h"
#include "AsepriteParser.h"
#include "D2DRender.h"
#include "AssetManager.h"

void AssetManager::LoadTexture(sample::D2DRenderer* d2dContext, const std::wstring& key, const std::filesystem::path& filePath)
{
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
    ID2D1Bitmap1* rawBitmap = nullptr;
    d2dContext->CreateBitmapFromFile(filePath.c_str(), rawBitmap);
    bitmap.Attach(rawBitmap);
    m_textures[key] = std::move(bitmap);
}

void AssetManager::LoadAseprite(sample::D2DRenderer* d2dContext, const std::wstring& key, const std::filesystem::path& filePath)
{
    AnimationClips clips = AsepriteParser::Load(filePath);

    std::filesystem::path imagePath = filePath;
    imagePath.replace_extension(L".png");
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;
    ID2D1Bitmap1* rawBitmap = nullptr;
    d2dContext->CreateBitmapFromFile(imagePath.c_str(), rawBitmap);
    bitmap.Attach(rawBitmap);

    for (auto& clip : clips)
    {
        clip.second.SetBitmap(bitmap);
    }

    m_clipsMap[key] = std::move(clips);
}

AnimationClips AssetManager::GetClips(const std::wstring& key) const
{
    auto it = m_clipsMap.find(key);
    if (it != m_clipsMap.end())
    {
        return it->second;
    }
    return AnimationClips();
}
