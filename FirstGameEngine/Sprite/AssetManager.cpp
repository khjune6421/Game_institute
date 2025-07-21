#include "AssetManager.h"
#include "AsepriteParser.h"

ID2D1Bitmap1* AssetManager::LoadTexture(Render* render, const wstring& key, const filesystem::path& filePath)
{
	auto it = m_textures.find(key);
	if (it != m_textures.end()) return it->second.Get();

	render->CreateBitmapFromFile(filePath.c_str(), m_textures[key].GetAddressOf());

	return m_textures[key].Get();
}

AnimationClips& AssetManager::LoadAnimationClips(Render* render, const wstring& key, const filesystem::path& filePath)
{
	auto it = m_animationClipsMap.find(key);
	if (it != m_animationClipsMap.end()) return it->second;

	ComPtr<ID2D1Bitmap1> bitmap = GetTexture(key);
	wstring keyName = key + L".png";
	render->CreateBitmapFromFile(keyName.c_str(), bitmap.GetAddressOf());

	auto tempMap = AsepriteParser::Load(filePath);

	AnimationClips clips;
	clips.reserve(tempMap.size());
	for (auto& [name, clip] : tempMap)
	{
		clip.SetBitmap(bitmap.Get());
		clips.emplace_back(move(name), move(clip));
	}

	auto result = m_animationClipsMap.emplace(key, move(clips));

	return result.first->second;
}

const AnimationClips& AssetManager::GetAnimationClips(const wstring& key) const
{
	static const AnimationClips emptyClips;

	auto it = m_animationClipsMap.find(key);
	if (it == m_animationClipsMap.end()) return emptyClips;

	return it->second;
}

ID2D1Bitmap1* AssetManager::GetTexture(const wstring& key) const
{
	auto it = m_textures.find(key);
	if (it == m_textures.end()) return nullptr;

	return it->second.Get();
}
