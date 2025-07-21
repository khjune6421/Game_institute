#pragma once
#include <unordered_map>
#include <wrl/client.h>
#include <string>
#include <d2d1_3.h>
#include <filesystem>
#include <wincodec.h>
#include "DirectX.h"
#include "AnimationClip.h"

namespace Renderer
{
	class DirectX;
}

using namespace std;
using namespace Microsoft::WRL;
using AnimationClips = vector<pair<string, AnimationClip>>;
using Render = Renderer::DirectX;

class AssetManager
{
	unordered_map<wstring, ComPtr<ID2D1Bitmap1>> m_textures;
	unordered_map<wstring, AnimationClips> m_animationClipsMap;

public:
	AssetManager() = default;

	ID2D1Bitmap1* LoadTexture(Render* render, const wstring& key, const filesystem::path& filePath);
	ID2D1Bitmap1* GetTexture(const wstring& key) const;

	AnimationClips& LoadAnimationClips(Render* render, const wstring& key, const filesystem::path& filePath);
	const AnimationClips& GetAnimationClips(const wstring& key) const;

	void Clear() { m_textures.clear(); m_animationClipsMap.clear(); }

	~AssetManager() = default;
};