#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"
#include <list>
#include <vector>
#include <wrl/client.h>
#include <d2d1_1.h>
#include "BoxObject.h"

class BoxObject;

class TransformPracticeScene : public OnlyForTestScene
{
public:
    TransformPracticeScene() = default;
    virtual ~TransformPracticeScene();
    
    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

    BoxObject* MakePlanet(ComPtr<ID2D1Bitmap1>& bitmap, Vec2 position, float rotationSpeed, BoxObject* parent = nullptr)
    {
		BoxObject* planet = new BoxObject(bitmap);
		if (parent)
		{
			planet->SetParent(parent);
		}
		planet->SetPosition(position);
        planet->SetRotationSpeed(rotationSpeed);
		m_BoxObjects.push_back(planet);
		return planet;
    };

protected:
    void ProcessKeyboardEvents() override;

private:
   
    UnityCamera m_UnityCamera;

	ComPtr<ID2D1Bitmap1> m_BitmapPtr[10];

	std::vector<BoxObject*> m_BoxObjects;
};

