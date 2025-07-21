#pragma once
#include <Windows.h>

LRESULT CALLBACK NzWndBaseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class NzWndBase
{
protected:
	friend LRESULT CALLBACK NzWndBaseProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual bool OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }

	virtual void OnResize(int width, int height);
	virtual void OnClose() abstract;

	HWND m_hWnd = HWND();
	int m_width = 0;
	int m_height = 0;

	NzWndBase(const NzWndBase&) = delete;
	NzWndBase& operator=(const NzWndBase&) = delete;
	NzWndBase(NzWndBase&&) = delete;
	NzWndBase& operator=(NzWndBase&&) = delete;

public:
	NzWndBase() = default;

	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);
	void Destroy();

	HWND GetHwnd() const { return m_hWnd; }
	void SetHwnd(HWND hWnd) { m_hWnd = hWnd; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	virtual ~NzWndBase() = default;
};