#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

#include <wrl/client.h>
#include <d3d11.h>
#include <d2d1_3.h>
#include <dxgi1_6.h>
#include <dwrite_3.h>
#include <wincodec.h>

using namespace Microsoft::WRL;

namespace Renderer
{
	class DirectX
	{
		HWND m_hWnd = nullptr;

		// Direct3D 11 ¸â¹ö º¯¼ö
		ComPtr<ID3D11Device> m_d3dDevice;
		ComPtr<ID3D11DeviceContext> m_d3dContext;
		ComPtr<ID3D11RenderTargetView> m_d3dRenderTargetView;

		// Direct2D ¸â¹ö º¯¼ö
		ComPtr<ID2D1Device7> m_d2dDevice;
		ComPtr<ID2D1DeviceContext7> m_d2dContext;
		ComPtr<ID2D1Bitmap1> m_d2dBitmap;
		ComPtr<ID2D1SolidColorBrush> m_d2dBrush;

		// DirectWrite ¸â¹ö º¯¼ö
		ComPtr<ID2D1SolidColorBrush> m_d2dTextBrush;
		ComPtr<IDWriteTextFormat> m_textFormat;

		// ±âÅ¸ DirectX ¸â¹ö º¯¼ö
		ComPtr<IDXGISwapChain1> m_swapChain;
		ComPtr<IWICImagingFactory> m_wicFactory;

		void CreateDevice(HWND hWnd);

		void CreateRenderTarget();
		void ReleaseRenderTarget();

		void CreateWriteResource();

	public:
		DirectX() = default;

		void Initialize(HWND hWnd);
		void Release();
		void Resize(UINT width, UINT height);

		void DrawLine(D2D1_POINT_2F start, D2D1_POINT_2F end, D2D1_COLOR_F color, float strokeWidth = 1.0f);
		void DrawCircle(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color, bool fill = false, float strokeWidth = 1.0f);
		void DrawRectangle(const D2D1_RECT_F* rect, D2D1_COLOR_F color, bool fill = false, float strokeWidth = 1.0f);

		void CreateBitmapFromFile(const wchar_t* filePath, ID2D1Bitmap1** bitmap);
		void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity = 1.0f);
		void DrawText(const wchar_t* text, const D2D1_RECT_F* layoutRect, D2D1_COLOR_F color);

		void SetTransform(const D2D1_MATRIX_3X2_F& transform);

		void RenderBegin();
		void RenderEnd(bool isPresent = true);
		void Present();

		ID3D11Device* Get3DDevice() const { return m_d3dDevice.Get(); }
		ID3D11DeviceContext* Get3DContext() const { return m_d3dContext.Get(); }
		ID3D11RenderTargetView* Get3DRenderTargetView() const { return m_d3dRenderTargetView.Get(); }

		~DirectX() { Release(); }
	};
}