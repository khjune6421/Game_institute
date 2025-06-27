#pragma once
#include <d2d1_2.h>
#include <wrl/client.h>
#include <string>
#include <iostream>
#include "TMHelper.h"
#include "D2DTransform.h"
#include "D2DRender.h"
#include "Camera2D.h"
#include <cassert>

using Microsoft::WRL::ComPtr;
using TestRenderer = myspace::D2DRenderer;
using Vec2 = MYHelper::Vector2F;

class BoxObject
{
	BoxObject() = delete;
	BoxObject(const BoxObject&) = delete;
	void operator=(const BoxObject&) = delete;

public:
	BoxObject(ComPtr<ID2D1Bitmap1>& bitmap)
	{
		m_BitmapPtr = bitmap;

		++s_id;
		m_name += std::to_wstring(s_id); // ID�� �̸��� �߰�

		m_renderTM = MYTM::MakeRenderMatrix(true);

		D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };

		//m_transform.SetPivotPreset(D2DTM::PivotPreset::TopLeft, size);
		//m_transform.SetPivotPreset(D2DTM::PivotPreset::BottomRight, size);
		m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size);
	}

	~BoxObject() = default;

	void Update(float deltaTime)
	{
		if (m_isSelfRotation)
		{
			m_transform.Rotate(deltaTime * m_rotationSpeed); // �ڱ� ȸ��
		}
	}

	void Draw(TestRenderer& testRender, D2D1::Matrix3x2F viewTM)
	{
		static  D2D1_RECT_F s_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

		D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

		D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

		D2D1::ColorF boxColor = D2D1::ColorF::LightGray;

		if (m_isLeader) boxColor = D2D1::ColorF::Red;
		else if (m_isSelected) boxColor = D2D1::ColorF::HotPink;

		testRender.SetTransform(finalTM);
		testRender.DrawRectangle(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom, boxColor);

		D2D1_RECT_F dest = D2D1::RectF(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom);

		testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
		testRender.DrawMessage(m_name.c_str(), s_rect.left, s_rect.top, 200, 50, D2D1::ColorF::Black);
	}

	void SetPosition(const Vec2& pos)
	{
		m_transform.SetPosition(pos);
	}

	void Move(const Vec2& offset)
	{
		m_transform.Translate(offset);
	}

	void Rotate(float angle)
	{
		m_transform.Rotate(angle);
	}

	void ToggleSelected()
	{
		m_isSelected = !m_isSelected;
	}

	bool IsSelected() const
	{
		return m_isSelected;
	}

	void ToggleSelfRotation()
	{
		m_isSelfRotation = !m_isSelfRotation;
	}

	void SetRotationSpeed(float rotationSpeed)
	{
		m_rotationSpeed = rotationSpeed;
	}

	bool IsHitTest(D2D1_POINT_2F worldPoint, D2D1::Matrix3x2F viewTM)
	{
		D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

		D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

		finalTM.Invert();

		// 2) ���� ��ǥ�� ����Ʈ ��ȯ
		D2D1_POINT_2F localPt = finalTM.TransformPoint(worldPoint);

		// 3) ���� �簢�� ����
		// (0,0) ~ (width, height) ������ �ִٸ� ��Ʈ!
		// m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

		std::cout << "BoxObject::IsHitTest: localPt = ("
			<< localPt.x << ", " << localPt.y << ")" << std::endl;

		std::cout << "BoxObject::IsHitTest: m_rect = ("
			<< m_rect.left << ", " << m_rect.top << ", "
			<< m_rect.right << ", " << m_rect.bottom << ")" << std::endl;

		// 4) ���� �������� �˻�
		return MYTM::IsPointInRect(localPt, m_rect);
	}

	D2DTM::Transform* GetTransform()
	{
		return &m_transform;
	}

	void SetParent(BoxObject* parent)
	{
		assert(parent != nullptr);

		if (nullptr != m_transform.GetParent())
		{
			// �̹� �θ� �ִٸ� �θ� ���踦 �����մϴ�.
			m_transform.DetachFromParent();
		}

		m_transform.SetParent(parent->GetTransform());
	}

	void DetachFromParent()
	{
		m_transform.DetachFromParent();
	}

	void SetLeader(bool isLeader)
	{
		m_isLeader = isLeader;
	}

private:
	D2DTM::Transform m_transform;

	MAT3X2F m_renderTM; // ������ ��ȯ ���

	D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

	std::wstring m_name = L"";

	bool m_isSelected = false;
	bool m_isLeader = false; // ���� �ڽ� ����

	bool m_isSelfRotation = true; // �ڱ� ȸ�� ����
	float m_rotationSpeed = 36.f;

	ComPtr<ID2D1Bitmap1> m_BitmapPtr;

	static int s_id; // static ��� ������ ID�� �����մϴ�. (������)

};