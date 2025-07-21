#include "AnimationClip.h"
#include <stdexcept>

void SpriteAnimator::SetCurrentClip(unsigned int index)
{
	if (m_clips && index < m_clips->size())
	{
		m_clip = &(*m_clips)[index].second;
		clipIndex = index;
	}
	else
	{
		m_clip = &(*m_clips)[0].second;
		clipIndex = 0;
	}
}

const AnimationClip* SpriteAnimator::GetCurrentClip() const
{
	if (m_clips && clipIndex < m_clips->size())
	{
		return &(*m_clips)[clipIndex].second;
	}
	return nullptr;
}

void SpriteAnimator::Update(float deltaTime)
{
	if (!m_clip || m_clip->GetFrames().empty()) throw std::runtime_error("애니메이션 클립이 유효하지 않거나 프레임이 없음.");

	m_elapsedTime += deltaTime;
	float totalDuration = m_clip->GetTotalDuration();

	if (m_isLoop)
	{
		if (m_elapsedTime >= totalDuration) m_elapsedTime = fmod(m_elapsedTime, totalDuration);
	}
	else if (m_elapsedTime > totalDuration)
	{
		m_elapsedTime = totalDuration;
	}
}

const Frame& SpriteAnimator::GetCurrentFrame() const
{
	static Frame fallbackDummy = { {0,0,0,0}, 0.0f };
	if (!m_clip || m_clip->GetFrames().empty()) return fallbackDummy;

	float accumulatedTime = 0.0f;
	for (const auto& frame : m_clip->GetFrames())
	{
		accumulatedTime += frame.duration;

		if (m_elapsedTime < accumulatedTime) return frame;
	}

	return m_clip->GetFrames().back();
}