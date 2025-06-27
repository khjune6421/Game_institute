#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "HierarchicalTransformTestScene.h"

// 
// HierarchicalTransformTestScene는 계층 구조를 테스트하는 씬입니다.
// 이 씬에서는 BoxObject를 사용하여 계층 구조를 구성하고, 카메라 이동 및 렌더링을 테스트합니다.

// 
// HierarchicalTransformTestScene는 유니티 좌표계 카메라를 사용합니다.
// F1키로 고양이 오브젝트를 생성하고, F2키로 계층 구조를 연결합니다.


HierarchicalTransformTestScene::~HierarchicalTransformTestScene()
{
    for (auto& box : m_BoxObjects)
    {
        delete box;
    }
}

void HierarchicalTransformTestScene::SetUp(HWND hWnd)
{
    constexpr int defaultGameObjectCount = 100;

    m_BoxObjects.reserve(defaultGameObjectCount);

    m_hWnd = hWnd;

    SetWindowText(m_hWnd, 
    L"[이동키] 카메라, [ASDW] [스페이스]고양이 이동과 회전, [L-Btn] 생성, [R-Btn] 선택, [F1] 클리어");

    std::cout << "오브젝트 트랜스폼 계층구조 테스트 씬이 설정되었습니다." << std::endl;
    std::cout << "선택된 순서대로 오브젝트 간의 계층 구조가 만들어 집니다." << std::endl;
    std::cout << "리더 오브젝트를 이동 및 회전하면서 계층구조로 인한 변환 결과를 확인하세요." << std::endl;

  
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/cat.png", *m_BitmapPtr.GetAddressOf());
    //SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/redbird1.png", *m_BitmapPtr.GetAddressOf());

    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        m_UnityCamera.SetScreenSize(w, h);
    }
}

void HierarchicalTransformTestScene::Tick(float deltaTime)
{
    // 입력  이벤트 처리
    ProcessMouseEvents();
    ProcessKeyboardEvents();

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
    globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
    globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Black);

    for (auto& box : m_BoxObjects)
    {
        box->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();
}

void HierarchicalTransformTestScene::OnResize(int width, int height)
{
    // 윈도우 크기 변경 시 카메라의 화면 크기를 업데이트
    m_UnityCamera.SetScreenSize(width, height);
}

void HierarchicalTransformTestScene::ProcessKeyboardEvents()
{
    // 클리어
    if (InputManager::Instance().GetKeyPressed(VK_F1))
    {
        ClearBoxObjects();
    }

    if (InputManager::Instance().GetKeyPressed(VK_F2))
    {
        SetBoxSelfRotation();
    }


    // 카메라 이동 처리, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17부터는 structured binding을 사용하여 더 간결하게 표현할 수 있습니다.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
			m_UnityCamera.Move(dir.x, dir.y);
        }
    }

    // 첫번째 선택된 박스를 이동
    static const std::vector<std::pair<int, Vec2>> kBoxMoves = {
      { 'D', {  1.f,  0.f } }, // D키로 오른쪽 이동
      { 'A', { -1.f,  0.f } }, // A키로 왼쪽 이동
      { 'W', {  0.f,  1.f } }, // W키로 위로 이동
      { 'S', {  0.f, -1.f } }, // S키로 아래로 이동
    };

    for (auto& [vk, dir] : kBoxMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
		{
			if (!m_SelectedBoxObjects.empty())
			{
				m_SelectedBoxObjects.front()->Move(dir);
			}
        }
    }

    // 첫번째 선택된 박스를 회전
    if (InputManager::Instance().GetKeyDown(VK_SPACE) && !m_SelectedBoxObjects.empty())
    {
        m_SelectedBoxObjects.front()->Rotate(1.f); // 각도 단위로 회전
    }
}

void HierarchicalTransformTestScene::OnMouseLButtonDown(D2D1_POINT_2F pos)
{
    AddBoxObjects(pos);
}

void HierarchicalTransformTestScene::OnMouseRButtonDown(D2D1_POINT_2F pos)
{
    SelectBoxObject(pos);
}

void HierarchicalTransformTestScene::AddBoxObjects(D2D1_POINT_2F point)
{
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    cameraTM.Invert();

    D2D1_POINT_2F worldPt = cameraTM.TransformPoint(point);

    BoxObject* pNewBox = new BoxObject(m_BitmapPtr);

    pNewBox->SetPosition(Vec2(worldPt.x, worldPt.y));

    m_BoxObjects.push_back(pNewBox);
}

void HierarchicalTransformTestScene::ClearBoxObjects()
{
    for (auto& box : m_BoxObjects)
    {
        delete box;
    }

    m_BoxObjects.clear();

    m_SelectedBoxObjects.clear();
}

void HierarchicalTransformTestScene::SelectBoxObject(D2D1_POINT_2F point)
{
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    std::list<BoxObject*> oldSelectedList = std::move(m_SelectedBoxObjects);

    std::cout << "size of oldSelectedList: " << oldSelectedList.size() << std::endl;
    

    for (auto& box : m_BoxObjects)
    {
        if (box->IsHitTest(point, cameraTM))
        {
            box->ToggleSelected();

            if (box->IsSelected()) m_SelectedBoxObjects.push_back(box); // 새로 선택된 박스 추가
        }
    }  

    // 기존 계층 관계 해제하고 선택된 박스 목록 재구성
    for (auto it = oldSelectedList.crbegin(); it != oldSelectedList.crend(); ++it)
    {
        (*it)->DetachFromParent();

        if ((*it)->IsSelected())
        { 
            m_SelectedBoxObjects.push_front(*it); 
        }
        else
        {
            (*it)->SetLeader(false);
        }
        
    }

    std::cout << "size of m_SelectedBoxObjects: " << m_SelectedBoxObjects.size() << std::endl;

    // 계층 구조 업데이트
    UpdateRelationship();
}


void HierarchicalTransformTestScene::SetBoxSelfRotation()
{
    for (auto& box : m_BoxObjects)
    {
        if (false == box->IsSelected())
        {
            box->ToggleSelfRotation(); 
        }
    }
}


void HierarchicalTransformTestScene::UpdateRelationship()
{
    auto it = m_SelectedBoxObjects.begin();

    if (it == m_SelectedBoxObjects.end()) return; // 선택된 박스가 없으면 종료

    (*it)->SetLeader(true); // 첫번째 박스를 리더로 설정

    if (m_SelectedBoxObjects.size() < 2) return; // 선택된 박스가 2개 미만이면 종료

    while (it != m_SelectedBoxObjects.end() && std::next(it) != m_SelectedBoxObjects.end())
    {
        BoxObject* parent = *it;
        BoxObject* child = *(std::next(it));

        child->SetParent(parent);

        it++;

        std::cout << "부모: " << parent->GetTransform()->GetPosition().x << ", " 
                  << parent->GetTransform()->GetPosition().y 
                  << " 자식: " << child->GetTransform()->GetPosition().x << ", " 
            << child->GetTransform()->GetPosition().y << std::endl;
    }
}
   