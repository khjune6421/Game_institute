#pragma once
#include <memory>
#include "Timer.h"
#include "DirectX.h"
#include "NzWndBase.h"
#include "AssetManager.h"
#include "AnimationClip.h"
#include "ObjectManager.h"
#include "SceneManager.h"

namespace Renderer
{
	class DirectX;
}

using namespace std;

constexpr int TOTAL_SCENES = 2;

class MainApp : public NzWndBase
{
	shared_ptr<Renderer::DirectX> m_renderer = nullptr;

	Timer m_timer;

	void UpdateTime() { m_timer.Tick(); }
	void UpdateInput() {}

	void Update() { SceneManager::GetInstance().Update(m_timer.GetDeltaTimeInMS()); }
	void Render() { SceneManager::GetInstance().Render(); }

	void OnResize(int width, int height) override;
	void OnClose() override;

public:
	MainApp() = default;

	bool Initialize();
	void Run();
	void Finalize();

	bool OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual ~MainApp() = default;
};