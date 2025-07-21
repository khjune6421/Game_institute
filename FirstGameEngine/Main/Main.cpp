#include "MainApp.h"

namespace
{
	MainApp* g_appInstance = nullptr;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) return -1;

	g_appInstance = new MainApp();

	if (!g_appInstance->Initialize()) throw std::runtime_error("MainApp 초기화 실패.");
	g_appInstance->Run();
	g_appInstance->Finalize();

	delete g_appInstance;

	CoUninitialize();
}