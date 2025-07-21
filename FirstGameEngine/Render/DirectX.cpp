#include "DirectX.h"
#include <stdexcept>

namespace Renderer
{
	void DirectX::CreateDevice(HWND hWnd)
	{
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };
		ComPtr<ID3D11Device> d3dDevice;
		ComPtr<ID3D11DeviceContext> d3dContext;

		HRESULT hr = D3D11CreateDevice
		(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&d3dDevice,
			nullptr,
			&d3dContext
		);
		if (FAILED(hr)) throw std::runtime_error("D3D11 생성 실패");

		ComPtr<IDXGIDevice> dxgiDevice;
		hr = d3dDevice.As(&dxgiDevice);
		if (FAILED(hr)) throw std::runtime_error("IDXGIDevice 변환 실패");

		ComPtr<IDXGIAdapter> dxgiAdapter;
		hr = dxgiDevice->GetAdapter(&dxgiAdapter);
		if (FAILED(hr)) throw std::runtime_error("IDXGIAdapter 가져오기 실패");

		ComPtr<IDXGIFactory2> dxgiFactory;
		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		if (FAILED(hr)) throw std::runtime_error("IDXGIFactory2 가져오기 실패");

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

		ComPtr<IDXGISwapChain1> swapChain;
		hr = dxgiFactory->CreateSwapChainForHwnd
		(
			d3dDevice.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		);
		if (FAILED(hr)) throw std::runtime_error("SwapChain 생성 실패");

		D2D1_FACTORY_OPTIONS d2dOptions = {};
		d2dOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

		ComPtr<ID2D1Factory8> d2dFactory;

		hr = D2D1CreateFactory
		(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory8),
			&d2dOptions,
			reinterpret_cast<void**>(d2dFactory.GetAddressOf())
		);
		if (FAILED(hr)) throw std::runtime_error("D2D1 팩토리 생성 실패");

		ComPtr<ID2D1Device> d2dDevice;
		hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
		if (FAILED(hr)) throw std::runtime_error("D2D1 디바이스 생성 실패");

		ComPtr<ID2D1Device7> d2dDevice7;
		hr = d2dDevice.As(&d2dDevice7);
		if (FAILED(hr)) throw std::runtime_error("D2D1 디바이스7 변환 실패");

		ComPtr<ID2D1DeviceContext7> d2dContext;
		hr = d2dDevice7->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
		if (FAILED(hr)) throw std::runtime_error("D2D1 디바이스 컨텍스트 생성 실패");

		m_d3dDevice = d3dDevice;
		m_d3dContext = d3dContext;

		m_d2dDevice = d2dDevice7;
		m_d2dContext = d2dContext;

		m_swapChain = swapChain;
	}

	void DirectX::CreateRenderTarget()
	{
		ComPtr<IDXGISurface> backBuffer;
		HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		if (FAILED(hr)) throw std::runtime_error("백버퍼 가져오기 실패");

		D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
		bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		bitmapProperties.dpiX = 96.0f;
		bitmapProperties.dpiY = 96.0f;

		ComPtr<ID2D1Bitmap1> d2dBitmap;
		hr = m_d2dContext->CreateBitmapFromDxgiSurface(backBuffer.Get(), &bitmapProperties, d2dBitmap.GetAddressOf());
		if (FAILED(hr)) throw std::runtime_error("D2D1 비트맵 생성 실패");

		m_d2dContext->SetTarget(d2dBitmap.Get());
		hr = m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_d2dBrush);
		if (FAILED(hr)) throw std::runtime_error("D2D1 브러시 생성 실패");

		ComPtr<ID3D11Texture2D> backBufferTexture;
		hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferTexture));
		if (FAILED(hr)) throw std::runtime_error("백버퍼 텍스처 가져오기 실패");

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		hr = m_d3dDevice->CreateRenderTargetView(backBufferTexture.Get(), nullptr, &renderTargetView);
		if (FAILED(hr)) throw std::runtime_error("렌더 타겟 뷰 생성 실패");

		m_d3dRenderTargetView = renderTargetView;
	}

	void DirectX::ReleaseRenderTarget()
	{
		if (m_d2dContext)
		{
			m_d2dContext->SetTarget(nullptr);
		}
		m_d3dRenderTargetView.Reset();
		m_d2dBitmap.Reset();
		m_d2dBrush.Reset();
		m_d2dTextBrush.Reset();
	}

	void DirectX::CreateWriteResource()
	{
		ComPtr<IDWriteFactory> dwriteFactory = nullptr;
		HRESULT hr = DWriteCreateFactory
		(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf())
		);
		if (FAILED(hr)) throw std::runtime_error("DWrite 팩토리 생성 실패");

		dwriteFactory->CreateTextFormat
		(
			L"Segoe UI",
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			16.0f,
			L"ko-KR",
			&m_textFormat
		);
		if (FAILED(hr)) throw std::runtime_error("DWrite 텍스트 포맷 생성 실패");

		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	}

	void DirectX::Initialize(HWND hWnd)
	{
		m_hWnd = hWnd;

		CreateDevice(m_hWnd);
		CreateRenderTarget();
		CreateWriteResource();

		ComPtr<IWICImagingFactory> wicFactory;

		HRESULT hr = CoCreateInstance
		(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);
		if (FAILED(hr)) throw std::runtime_error("WIC Imaging Factory 생성 실패");

		m_wicFactory = wicFactory;
	}

	void DirectX::Release()
	{
		ReleaseRenderTarget();

		m_d3dDevice = nullptr;

		m_d2dDevice = nullptr;
		m_d2dContext = nullptr;
		m_d2dBrush = nullptr;

		m_wicFactory = nullptr;
		m_d2dBitmap = nullptr;

		m_swapChain = nullptr;
	}

	void DirectX::Resize(UINT width, UINT height)
	{
		if (!m_swapChain) throw std::runtime_error("SwapChain이 초기화되지 않음.");

		ReleaseRenderTarget();

		HRESULT hr = m_swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (FAILED(hr)) throw std::runtime_error("SwapChain 리사이즈 실패");

		CreateRenderTarget();
	}

	void DirectX::DrawLine(D2D1_POINT_2F start, D2D1_POINT_2F end, D2D1_COLOR_F color, float strokeWidth)
	{
		m_d2dBrush->SetColor(color);

		m_d2dContext->DrawLine(start, end, m_d2dBrush.Get(), strokeWidth);
	}

	void DirectX::DrawCircle(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color, bool fill, float strokeWidth)
	{
		m_d2dBrush->SetColor(color);

		if (fill) m_d2dContext->FillEllipse(D2D1::Ellipse(center, radius, radius), m_d2dBrush.Get());
		else m_d2dContext->DrawEllipse(D2D1::Ellipse(center, radius, radius), m_d2dBrush.Get(), strokeWidth);
	}

	void DirectX::DrawRectangle(const D2D1_RECT_F* rect, D2D1_COLOR_F color, bool fill, float strokeWidth)
	{
		m_d2dBrush->SetColor(color);

		if (fill) m_d2dContext->FillRectangle(*rect, m_d2dBrush.Get());
		else m_d2dContext->DrawRectangle(*rect, m_d2dBrush.Get(), strokeWidth);
	}

	void DirectX::CreateBitmapFromFile(const wchar_t* filePath, ID2D1Bitmap1** bitmap)
	{
		ComPtr<IWICBitmapDecoder> decoder;
		HRESULT hr = m_wicFactory->CreateDecoderFromFilename
		(
			filePath,
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&decoder
		);
		if (FAILED(hr)) throw std::runtime_error("WIC 디코더 생성 실패");

		ComPtr<IWICBitmapFrameDecode> frame;
		hr = decoder->GetFrame(0, &frame);
		if (FAILED(hr)) throw std::runtime_error("WIC 프레임 디코드 실패");

		ComPtr<IWICFormatConverter> converter;
		hr = m_wicFactory->CreateFormatConverter(&converter);
		if (FAILED(hr)) throw std::runtime_error("WIC 포맷 컨버터 생성 실패");

		hr = converter->Initialize
		(
			frame.Get(),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0f,
			WICBitmapPaletteTypeCustom
		);
		if (FAILED(hr)) throw std::runtime_error("WIC 포맷 컨버터 초기화 실패");

		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1
		(
			D2D1_BITMAP_OPTIONS_NONE,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);

		hr = m_d2dContext->CreateBitmapFromWicBitmap
		(
			converter.Get(),
			&bitmapProperties,
			bitmap
		);
		if (FAILED(hr)) throw std::runtime_error("D2D1 비트맵 생성 실패");
	}

	void DirectX::DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity)
	{
		if (!bitmap) throw std::runtime_error("비트맵이 nullptr임.");

		m_d2dContext->DrawBitmap
		(
			bitmap,
			destRect,
			opacity,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			srcRect
		);
	}

	void DirectX::DrawText(const wchar_t* text, const D2D1_RECT_F* layoutRect, D2D1_COLOR_F color)
	{
		if (!text || !layoutRect) throw std::runtime_error("텍스트나 레이아웃 사각형이 nullptr임.");

		m_d2dTextBrush->SetColor(color);
		m_d2dContext->DrawText
		(
			text,
			wcslen(text),
			m_textFormat.Get(),
			*layoutRect,
			m_d2dTextBrush.Get()
		);
	}

	void DirectX::SetTransform(const D2D1_MATRIX_3X2_F& transform)
	{
		if (!m_d2dContext) throw std::runtime_error("D2D1 컨텍스트가 nullptr임.");

		m_d2dContext->SetTransform(transform);
	}

	void DirectX::RenderBegin()
	{
		m_d2dContext->BeginDraw();
		m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Cyan));
	}

	void DirectX::RenderEnd(bool isPresent)
	{
		HRESULT hr = m_d2dContext->EndDraw();

		if (FAILED(hr) && hr != D2DERR_RECREATE_TARGET) throw std::runtime_error("D2D1 드로우 종료 실패");

		if (isPresent)
		{
			Present();
		}
	}

	void DirectX::Present()
	{
		if (!m_swapChain) throw std::runtime_error("SwapChain이 초기화되지 않았음.");

		HRESULT hr = m_swapChain->Present(1, 0);
		if (FAILED(hr)) throw std::runtime_error("SwapChain 프레젠트 실패");

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			Release();
			Initialize(m_hWnd);
		}
		else
		{
			if (FAILED(hr)) throw std::runtime_error("SwapChain 프레젠트 중 오류 발생");
		}
	}
}