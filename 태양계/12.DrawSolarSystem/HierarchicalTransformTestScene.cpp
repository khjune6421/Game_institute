#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "HierarchicalTransformTestScene.h"

// 
// HierarchicalTransformTestScene�� ���� ������ �׽�Ʈ�ϴ� ���Դϴ�.
// �� �������� BoxObject�� ����Ͽ� ���� ������ �����ϰ�, ī�޶� �̵� �� �������� �׽�Ʈ�մϴ�.

// 
// HierarchicalTransformTestScene�� ����Ƽ ��ǥ�� ī�޶� ����մϴ�.
// F1Ű�� ����� ������Ʈ�� �����ϰ�, F2Ű�� ���� ������ �����մϴ�.


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
    L"[�̵�Ű] ī�޶�, [ASDW] [�����̽�]����� �̵��� ȸ��, [L-Btn] ����, [R-Btn] ����, [F1] Ŭ����");

    std::cout << "������Ʈ Ʈ������ �������� �׽�Ʈ ���� �����Ǿ����ϴ�." << std::endl;
    std::cout << "���õ� ������� ������Ʈ ���� ���� ������ ����� ���ϴ�." << std::endl;
    std::cout << "���� ������Ʈ�� �̵� �� ȸ���ϸ鼭 ���������� ���� ��ȯ ����� Ȯ���ϼ���." << std::endl;

  
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
    // �Է�  �̺�Ʈ ó��
    ProcessMouseEvents();
    ProcessKeyboardEvents();

    for (auto& box : m_BoxObjects)
    {
        box->Update(deltaTime);
    }

    // ī�޶� ������Ʈ

    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true); // ī�޶� ��ġ ������ ��Ʈ����

    MAT3X2F finalTM = renderTM * cameraTM;

    // ������

    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    // ī�޶� ��ġ ǥ��
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
    // ������ ũ�� ���� �� ī�޶��� ȭ�� ũ�⸦ ������Ʈ
    m_UnityCamera.SetScreenSize(width, height);
}

void HierarchicalTransformTestScene::ProcessKeyboardEvents()
{
    // Ŭ����
    if (InputManager::Instance().GetKeyPressed(VK_F1))
    {
        ClearBoxObjects();
    }

    if (InputManager::Instance().GetKeyPressed(VK_F2))
    {
        SetBoxSelfRotation();
    }


    // ī�޶� �̵� ó��, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17���ʹ� structured binding�� ����Ͽ� �� �����ϰ� ǥ���� �� �ֽ��ϴ�.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
			m_UnityCamera.Move(dir.x, dir.y);
        }
    }

    // ù��° ���õ� �ڽ��� �̵�
    static const std::vector<std::pair<int, Vec2>> kBoxMoves = {
      { 'D', {  1.f,  0.f } }, // DŰ�� ������ �̵�
      { 'A', { -1.f,  0.f } }, // AŰ�� ���� �̵�
      { 'W', {  0.f,  1.f } }, // WŰ�� ���� �̵�
      { 'S', {  0.f, -1.f } }, // SŰ�� �Ʒ��� �̵�
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

    // ù��° ���õ� �ڽ��� ȸ��
    if (InputManager::Instance().GetKeyDown(VK_SPACE) && !m_SelectedBoxObjects.empty())
    {
        m_SelectedBoxObjects.front()->Rotate(1.f); // ���� ������ ȸ��
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

            if (box->IsSelected()) m_SelectedBoxObjects.push_back(box); // ���� ���õ� �ڽ� �߰�
        }
    }  

    // ���� ���� ���� �����ϰ� ���õ� �ڽ� ��� �籸��
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

    // ���� ���� ������Ʈ
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

    if (it == m_SelectedBoxObjects.end()) return; // ���õ� �ڽ��� ������ ����

    (*it)->SetLeader(true); // ù��° �ڽ��� ������ ����

    if (m_SelectedBoxObjects.size() < 2) return; // ���õ� �ڽ��� 2�� �̸��̸� ����

    while (it != m_SelectedBoxObjects.end() && std::next(it) != m_SelectedBoxObjects.end())
    {
        BoxObject* parent = *it;
        BoxObject* child = *(std::next(it));

        child->SetParent(parent);

        it++;

        std::cout << "�θ�: " << parent->GetTransform()->GetPosition().x << ", " 
                  << parent->GetTransform()->GetPosition().y 
                  << " �ڽ�: " << child->GetTransform()->GetPosition().x << ", " 
            << child->GetTransform()->GetPosition().y << std::endl;
    }
}
   