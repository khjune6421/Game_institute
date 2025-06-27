#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"

//
// class DefaultTestScene
// 
// 謝ル啗縑 渠п 檜пп 瑣衛棻.
// Top-Left Origin (0,0)    Bottom-Left Origin (0,0)    Center Origin (0,0)
// 忙(0,0)式式式Ⅱ X        ～ Y                              ～ Y
// 弛                       弛                                弛
// 弛                       弛                            式式式式托式式式式Ⅱ X
// ∪ Y                (0,0)戌式式式式Ⅱ X                         弛(0,0)
// 

class DefaultTestScene : public OnlyForTestScene
{
public:
    DefaultTestScene() = default;
    ~DefaultTestScene() override = default;

    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

private:

    void ProcessKeyboardEvents() override;

    void FlipCameraMode();
    void FlipRenderMode();

    MAT3X2F GetCameraTransform();

    std::wstring& GetTitleString() const;

    bool m_IsUnityCamera = false;
    bool m_IsRenerTMOn  = false;

    D2DCamera2D m_d2dCamera;
    UnityCamera m_unityCamera;
};


