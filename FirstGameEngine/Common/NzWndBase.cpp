#include "NzWndBase.h"
#include <stdexcept>

LRESULT NzWndBaseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NzWndBase* pWndBase = reinterpret_cast<NzWndBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (pWndBase)
	{
		if (pWndBase->OnWndProc(hWnd, uMsg, wParam, lParam)) return 0;
	}

	switch (uMsg)
	{
	case WM_SIZE:
	{
		pWndBase->OnResize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_CLOSE:
	{
		pWndBase->OnClose();
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void NzWndBase::OnResize(int width, int height)
{
	m_width = width;
	m_height = height;

	if (m_hWnd)
	{
		RECT rect;
		GetClientRect(m_hWnd, &rect);

		if (rect.right != width || rect.bottom != height)
		{
			SetWindowPos(m_hWnd, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}

bool NzWndBase::Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = className;
	wc.lpfnWndProc = NzWndBaseProc;

	ATOM classId = 0;
	if (!GetClassInfoEx(HINSTANCE(), className, &wc))
	{
		classId = RegisterClassEx(&wc);
		if (!classId) return false;
	}

	m_width = width;
	m_height = height;

	RECT rect = { 0, 0, width, height };

	AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

	m_hWnd = CreateWindowEx
	(
		0,
		MAKEINTATOM(classId),
		L"",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		HWND(),
		HMENU(),
		HINSTANCE(),
		nullptr
	);

	if (!m_hWnd) throw std::runtime_error("윈도우 생성 실패");

	SetWindowText(m_hWnd, windowName);
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}

void NzWndBase::Destroy()
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}