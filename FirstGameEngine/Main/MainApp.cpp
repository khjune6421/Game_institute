#include "MainApp.h"
#include <iostream>
#include <shobjidl.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "BaseEditScene.h"

void MainApp::OnResize(int width, int height)
{
	NzWndBase::OnResize(width, height);
	if (m_renderer) m_renderer->Resize(width, height);
}

void MainApp::OnClose()
{
	std::cout << "MainApp::OnClose() 호줄됨. 아직 기능 없음." << std::endl;
}

bool MainApp::Initialize()
{
	const wchar_t* className = L"MainAppWindowClass";
	const wchar_t* windowName = L"Main Application";

	if (!NzWndBase::Create(className, windowName, 1920, 1080)) throw std::runtime_error("윈도우 생성 실패.");

	m_renderer = make_shared<Renderer::DirectX>();
	m_renderer->Initialize(m_hWnd);

	ID3D11Device* pd3dDevice = m_renderer->Get3DDevice();
	ID3D11DeviceContext* pd3dDeviceContext = m_renderer->Get3DContext();

	auto& sceneManager = SceneManager::GetInstance();
	sceneManager.SetRenderer(m_renderer.get());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);
	ImGui_ImplWin32_Init(m_hWnd);

	m_timer.Reset();

	Scene* initScene = new BaseEditScene();
	sceneManager.sceneNames[0] = "BaseEditScene";
	sceneManager.sceneMap[sceneManager.sceneNames[0]] = std::shared_ptr<Scene>(initScene);
	sceneManager.SetScene(sceneManager.sceneMap[sceneManager.sceneNames[0]]);

	Scene* secondScene = new BaseEditScene();
	sceneManager.sceneNames[1] = "SecondScene";
	sceneManager.sceneMap[sceneManager.sceneNames[1]] = std::shared_ptr<Scene>(secondScene);
	sceneManager.SetScene(sceneManager.sceneMap[sceneManager.sceneNames[1]]);

	sceneManager.ChangeScene(sceneManager.sceneNames[0]);

	return true;
}

void MainApp::Run()
{
	MSG msg = { nullptr };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			UpdateTime();
			UpdateInput();
			Update();
			Render();
		}
	}
}

void MainApp::Finalize()
{
	if (m_renderer)
	{
		m_renderer->Release();
		m_renderer.reset();
	}

	auto& sceneManager = SceneManager::GetInstance();
	sceneManager.EndScene(sceneManager.sceneMap[sceneManager.sceneNames[0]]);
	sceneManager.EndScene(sceneManager.sceneMap[sceneManager.sceneNames[1]]);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool MainApp::OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	return false;
}