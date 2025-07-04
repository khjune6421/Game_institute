#include "pch.h"
#include <fstream>
#include "json.hpp"
#include "AsepriteParser.h"

static std::map<std::filesystem::path, AnimationClip> loadedClips;

AnimationClips AsepriteParser::Load(const std::filesystem::path& jsonPath)
{
    // 1) JSON 로드하세요.
    std::ifstream file(jsonPath);
    if (!file.is_open()) { throw std::runtime_error("파일 열기 실패: " + jsonPath.string()); } // 파일 열기 실패 시 예외 발생

    std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto parsedJsonData = nlohmann::json::parse(jsonContent, nullptr, false);
    if (parsedJsonData.is_discarded()) { throw std::runtime_error("파일 변환 실패: " + jsonPath.string()); } // JSON 파싱 실패 시 예외 발생

    // 2) 태그별로  AnimationClips 생성합니다.
    AnimationClips clips;

    const auto& root = parsedJsonData;
    auto& frames = root["frames"];

    if (root["meta"].contains("frameTags"))
    {
        for (const auto& t : root["meta"]["frameTags"])
        {
            if (loadedClips.find(jsonPath) != loadedClips.end()) // 이미 로드된 적이 있다면
            {
                AnimationClip clip;
                clip = loadedClips[jsonPath];
            }
            else if (frames.is_object())
            {
                for (auto it = frames.begin(); it != frames.end(); ++it)
                {
                    AnimationClip clip;
                    const auto& frameData = it.value();
                    Frame frame;
                    frame.duration = frameData["duration"];
                    frame.srcRect.left = frameData["frame"]["x"];
                    frame.srcRect.top = frameData["frame"]["y"];
                    frame.srcRect.right = frame.srcRect.left + frameData["frame"]["w"];
                    frame.srcRect.bottom = frame.srcRect.top + frameData["frame"]["h"];
                    clip.AddFrame(frame);
                    loadedClips[jsonPath] = clip; // 로드된 클립 저장
                    clips.push_back({ t["name"], clip });
                }
            }
            else if (frames.is_array())
            {
                for (const auto& frameData : frames)
                {
                    AnimationClip clip;
                    Frame frame;
                    frame.duration = frameData["duration"];
                    frame.srcRect.left = frameData["frame"]["x"];
                    frame.srcRect.top = frameData["frame"]["y"];
                    frame.srcRect.right = frame.srcRect.left + frameData["frame"]["w"];
                    frame.srcRect.bottom = frame.srcRect.top + frameData["frame"]["h"];
                    clip.AddFrame(frame);
                    loadedClips[jsonPath] = clip;
                    clips.push_back({ t["name"], clip });
                }
            }
        }
    }

    return clips;
}
