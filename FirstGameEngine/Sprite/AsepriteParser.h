#pragma once
#include <filesystem>
#include "AnimationClip.h"

using namespace std;
using AnimationClips = vector<pair<string, AnimationClip>>;

class AsepriteParser
{
public:
	AsepriteParser() = default;

	static AnimationClips Load(const filesystem::path& path);

	~AsepriteParser() = default;
};