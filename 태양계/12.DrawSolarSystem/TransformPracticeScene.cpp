#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "TransformPracticeScene.h"

TransformPracticeScene::~TransformPracticeScene()
{
	for (auto& box : m_BoxObjects)
	{
		delete box;
	}
}

void TransformPracticeScene::SetUp(HWND hWnd)
{
	constexpr int defaultGameObjectCount = 100;

	m_BoxObjects.reserve(defaultGameObjectCount);

    m_hWnd = hWnd;

    SetWindowText(m_hWnd, 
    L"가상의 태양계를 만들어 주세요. 물리 법칙은 무시 합니다. ^^;;");

	std::cout << "태양은 자전을 해야 합니다." << std::endl;
    std::cout << "행성들은 자전을 하며 동시에 태영의 자전에 영향을 받아 공전하는 것처럼 보입니다."<< std::endl;
    std::cout << "달은 자전을 하면서 동시에 지구의 자전에 영향을 받아 공전하는 것처럼 보입니다." << std::endl;
    std::cout << "회전 속도는 자유롭게 설정하세요." << std::endl;

	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/sun.png", *m_BitmapPtr[0].GetAddressOf());
	BoxObject* sun = new BoxObject(m_BitmapPtr[0]);
	sun->SetPosition(Vec2(0.f, 0.f));
	sun->SetRotationSpeed(50.f);
	m_BoxObjects.push_back(sun);

	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/earth.png", *m_BitmapPtr[1].GetAddressOf());
	BoxObject* earth = new BoxObject(m_BitmapPtr[1]);
	earth->SetParent(sun);
	earth->SetPosition(Vec2(400.f, 0.f));
	earth->SetRotationSpeed(200.f);
	m_BoxObjects.push_back(earth);

	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/moon.png", *m_BitmapPtr[2].GetAddressOf());
	BoxObject* moon = new BoxObject(m_BitmapPtr[2]);
	moon->SetParent(earth);
	moon->SetPosition(Vec2(200.f, 0.f));
	moon->SetRotationSpeed(200.f);
	m_BoxObjects.push_back(moon);

	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/mars.png", *m_BitmapPtr[3].GetAddressOf());
	BoxObject* mars = new BoxObject(m_BitmapPtr[3]);
	mars->SetParent(sun);
	mars->SetPosition(Vec2(-500.f, 0.f));
	mars->SetRotationSpeed(-150.f);
	m_BoxObjects.push_back(mars);

	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/deimos.png", *m_BitmapPtr[4].GetAddressOf());
	BoxObject* deimos = new BoxObject(m_BitmapPtr[4]);
	deimos->SetParent(mars);
	deimos->SetPosition(Vec2(200.f, 0.f));
	deimos->SetRotationSpeed(300.f);
	m_BoxObjects.push_back(deimos);
   
    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        m_UnityCamera.SetScreenSize(w, h);
    }
}

void TransformPracticeScene::Tick(float deltaTime)
{
	for (auto& box : m_BoxObjects)
	{
		box->Update(deltaTime);
	}

	// 카메라 업데이트

	MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

	MAT3X2F renderTM = MYTM::MakeRenderMatrix(true); // 카메라 위치 렌더링 매트릭스

	MAT3X2F finalTM = renderTM * cameraTM;

	// 렌더링

	static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

	wchar_t buffer[128] = L"";
	MYTM::MakeMatrixToString(cameraTM, buffer, 128);

	globalRenderer.RenderBegin();

	globalRenderer.SetTransform(finalTM);

	// 카메라 위치 표시
	//globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
	//globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
	//globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Black);

	for (auto& box : m_BoxObjects)
	{
		box->Draw(globalRenderer, cameraTM);
	}

	globalRenderer.RenderEnd();
}

void TransformPracticeScene::OnResize(int width, int height)
{
	m_UnityCamera.SetScreenSize(width, height);
}

void TransformPracticeScene::ProcessKeyboardEvents()
{
}