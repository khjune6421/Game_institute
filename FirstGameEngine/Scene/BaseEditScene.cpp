#include "BaseEditScene.h"
#include <stdexcept>
#include <filesystem>
#include <shobjidl.h>
#include <fstream>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "AssetManager.h"

static std::wstring ConvertToWString(const std::string& str)
{
	size_t len = 0;
	mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
	if (len == 0)
		return L"";

	std::wstring wstr(len, L'\0');
	mbstowcs_s(&len, &wstr[0], len, str.c_str(), _TRUNCATE);
	wstr.resize(len - 1); // Remove the null terminator added by mbstowcs_s  
	return wstr;
}

static std::string WStringToString(const std::wstring& wstr)
{
	size_t len = 0;
	wcstombs_s(&len, nullptr, 0, wstr.c_str(), _TRUNCATE);
	if (len == 0)
		return "";
	std::string str(len, '\0');
	wcstombs_s(&len, &str[0], len, wstr.c_str(), _TRUNCATE);
	str.resize(len - 1); // Remove the null terminator added by wcstombs_s
	return str;
}

static std::wstring SaveFile(const wchar_t* defaultName = L"scene_save.txt")
{
	HRESULT hr;
	IFileSaveDialog* pDialog = nullptr;
	std::wstring resultPath;

	hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog));
	if (FAILED(hr)) throw std::runtime_error("파일 저장 대화 상자 생성 실패.");

	pDialog->SetFileName(defaultName);

	COMDLG_FILTERSPEC fileTypes[] =
	{
		{ L"Text Files", L"*.txt" },
		{ L"All Files", L"*.*" }
	};
	pDialog->SetFileTypes(2, fileTypes);

	hr = pDialog->Show(nullptr);
	if (SUCCEEDED(hr))
	{
		IShellItem* pItem = nullptr;
		hr = pDialog->GetResult(&pItem);
		if (SUCCEEDED(hr) && pItem)
		{
			PWSTR filePath = nullptr;
			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
			if (SUCCEEDED(hr) && filePath)
			{
				resultPath = filePath;
				CoTaskMemFree(filePath);
			}
			pItem->Release();
		}
	}
	pDialog->Release();
	return resultPath;
}

void BaseEditScene::Initialize()
{
	pObjectManager = std::make_unique<ObjectManager>();
	m_pObjectManager = pObjectManager.get();

	ID3D11Device* pd3dDevice = m_renderer->Get3DDevice();
	ID3D11DeviceContext* pd3dDeviceContext = m_renderer->Get3DContext();
}

void BaseEditScene::Update(float deltaTime)
{
	unsigned int objectCount = m_pObjectManager->GetObjectCount();
	for (unsigned int i = 0; i < objectCount; ++i)
	{
		Object::ObjectId objectId = static_cast<Object::ObjectId>(i);
		Object* object = m_pObjectManager->GetObject(objectId);
		if (!object) continue;

		SpriteAnimator* spritePtr = m_pObjectManager->GetSpriteAnimator(objectId);
		if (spritePtr && spritePtr->IsValid())
		{
			spritePtr->Update(deltaTime);
		}
	}
}

void BaseEditScene::Render()
{
	if (!m_renderer) throw std::runtime_error("렌더러가 nullptr임.");
	m_renderer->RenderBegin();

	unsigned int objectCount = m_pObjectManager->GetObjectCount();
	for (unsigned int i = 0; i < objectCount; ++i)
	{
		Object::ObjectId objectId = static_cast<Object::ObjectId>(i);
		Object* object = m_pObjectManager->GetObject(objectId);
		if (!object) continue;

		SpriteAnimator* spritePtr = m_pObjectManager->GetSpriteAnimator(objectId);
		if (!spritePtr || !spritePtr->IsValid()) continue;

		const Frame& frame = spritePtr->GetCurrentFrame();
		Transform* transformPtr = m_pObjectManager->GetTransform(objectId);
		if (!transformPtr) continue;

		// 나중에 setDirty로 최적화
		D2D1_POINT_2F pivot = { transformPtr->x, transformPtr->y };
		float scaleX = transformPtr->scaleX;
		float scaleY = transformPtr->scaleY;
		float rotation = transformPtr->rotation;

		auto transform =
			D2D1::Matrix3x2F::Scale(scaleX, scaleY, pivot) *
			D2D1::Matrix3x2F::Rotation(rotation, pivot);

		m_renderer->SetTransform(transform);

		float xOffset = transformPtr->x;
		float yOffset = transformPtr->y;

		D2D1_RECT_F destRect = D2D1::RectF
		(
			pivot.x,
			pivot.y,
			pivot.x + frame.Width(),
			pivot.y + frame.Height()
		);

		m_renderer->DrawBitmap
		(
			spritePtr->GetCurrentClip()->GetBitmap(),
			destRect,
			frame.ToRectF()
		);

		m_renderer->SetTransform(D2D1::Matrix3x2F::Identity());
	}

	m_renderer->RenderEnd(false);
	RenderGUI();
	m_renderer->Present();
}

void BaseEditScene::RenderGUI()
{
	ID3D11DeviceContext* d3dDeviceContext = nullptr;
	d3dDeviceContext = m_renderer->Get3DContext();
	ID3D11RenderTargetView* renderTargetViews[] = { m_renderer->Get3DRenderTargetView() };
	if (!d3dDeviceContext || !renderTargetViews[0]) throw std::runtime_error("D3D11 디바이스 컨텍스트 또는 렌더 타겟 뷰가 nullptr임.");

	m_renderer->Get3DContext()->OMSetRenderTargets(1, renderTargetViews, nullptr);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Scenes"))
		{
			if (ImGui::MenuItem("Browse Scenes"))
			{
				m_showScenePanel = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Browse Folder"))
			{
				m_showFolderPanel = true;
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				SaveScene();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	if (m_showScenePanel) ScenePanel(&m_showScenePanel);
	if (m_showFolderPanel) FolderPanel(&m_showFolderPanel);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void BaseEditScene::ScenePanel(bool* pShowPanel)
{
	ImGui::Begin("Scene List", pShowPanel, ImGuiWindowFlags_AlwaysAutoResize);
	auto& sceneManager = SceneManager::GetInstance();
	for (const auto& sceneName : sceneManager.sceneNames)
	{
		if (ImGui::Button(sceneName.c_str()))
		{
			auto it = sceneManager.sceneMap.find(sceneName);
			if (it != sceneManager.sceneMap.end())
			{
				isSceneChanged = true;
				changeSceneName = sceneName;
			}
		}
		ImGui::SameLine();
	}
	ImGui::End();
}

void BaseEditScene::FolderPanel(bool* pShowPanel)
{
	ImGui::Begin("Browse Folder", pShowPanel, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Browse"))
	{
		BrowseFolder();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		if (!m_folderPath.empty())
		{
			UpdateFileList();
			m_selectedFile.clear();
		}
		UpdateFileList();
	}
	ImGui::Text("Selected Folder: %s", m_folderPath.c_str());

	// 오브젝트 생성
	if (!m_folderPath.empty())
	{
		ImGui::InputFloat("X", &m_x, 0.0f, 0.0f);
		ImGui::InputFloat("Y", &m_y, 0.0f, 0.0f);
		ImGui::InputFloat("Scale X", &m_scaleX, 0.0f, 0.0f);
		ImGui::InputFloat("Scale Y", &m_scaleY, 0.0f, 0.0f);
		ImGui::InputFloat("Rotation", &m_rotation, 0.0f, 0.0f);
		ImGui::InputInt("Clip Index", &clipIndex, 0, 0);

		if (ImGui::BeginListBox("Files", ImVec2(300.0f, 8 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const auto& file : m_fileList)
			{
				bool isSelected = (file == m_selectedFile);
				if (ImGui::Selectable(WStringToString(file).c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick)) m_selectedFile = file;
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Create New Object"))
		{
			Object::ObjectId newObjectId = m_pObjectManager->CreateObject();

			const AnimationClips& clips = LoadAsset(m_selectedFile);
			if (!clips.empty())
			{
				SpriteAnimator newSprite;
				newSprite.SetClips(&clips);
				newSprite.SetCurrentClip("TagEat");
				m_pObjectManager->AddSpriteAnimator(newObjectId, newSprite);
			}

			const Transform newTransform(m_x, m_y, m_scaleX, m_scaleY, m_rotation);
			m_pObjectManager->AddTransform(newObjectId, newTransform);

			wstring fullJsonPath = (filesystem::path(m_folderPath) / m_selectedFile).wstring();
			m_pObjectManager->SetObjectJsonPath(newObjectId, fullJsonPath);
		}
	}
	ImGui::End();
}

void BaseEditScene::BrowseFolder()
{
	HRESULT hr;
	IFileOpenDialog* pDialog = nullptr;

	hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog));
	if (FAILED(hr)) throw std::runtime_error("파일 열기 대화 상자 생성 실패.");

	DWORD options = 0;
	hr = pDialog->GetOptions(&options);
	if (FAILED(hr)) throw std::runtime_error("파일 열기 대화 상자 옵션 가져오기 실패.");

	pDialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
	hr = pDialog->Show(nullptr);
	if (FAILED(hr)) return;

	IShellItem* pItem = nullptr;
	hr = pDialog->GetResult(&pItem);
	if (FAILED(hr) || !pItem) throw std::runtime_error("파일 열기 대화 상자 결과 가져오기 실패.");

	PWSTR folderPath = nullptr;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &folderPath);
	if (FAILED(hr) || !folderPath) throw std::runtime_error("폴더 경로 가져오기 실패.");

	m_folderPath = folderPath;
	CoTaskMemFree(folderPath);

	pItem->Release();
}

void BaseEditScene::UpdateFileList()
{
	m_fileList.clear();

	for (const auto& entry : filesystem::directory_iterator(m_folderPath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension() == L".json")
			{
				m_fileList.push_back(entry.path().filename().wstring());
			}
		}
	}
}

void BaseEditScene::SaveScene()
{
	wstring savePath = SaveFile();
	if (savePath.empty()) return;

	ofstream outFile(savePath, ios::out | ios::binary);

	unsigned int objectCount = m_pObjectManager->GetObjectCount();
	outFile.write(reinterpret_cast<const char*>(&objectCount), sizeof(unsigned int));

	for (unsigned int i = 0; i < objectCount; ++i)
	{
		Object* object = m_pObjectManager->GetObject(i);
		if (!object) continue;

		outFile.write(reinterpret_cast<const char*>(&i), sizeof(Object::ObjectId));

		size_t pathLength = WStringToString(m_pObjectManager->GetObjectJsonPath(i)).length();
		outFile.write(reinterpret_cast<const char*>(&pathLength), sizeof(size_t));
		outFile.write(WStringToString(m_pObjectManager->GetObjectJsonPath(i)).c_str(), pathLength);

		Transform* transformPtr = m_pObjectManager->GetTransform(i);
		if (transformPtr)
		{
			outFile.write(reinterpret_cast<const char*>(&transformPtr->x), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&transformPtr->y), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&transformPtr->scaleX), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&transformPtr->scaleY), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&transformPtr->rotation), sizeof(float));
		}
		else
		{
			float defaultValue = 0.0f;
			outFile.write(reinterpret_cast<const char*>(&defaultValue), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&defaultValue), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&defaultValue), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&defaultValue), sizeof(float));
			outFile.write(reinterpret_cast<const char*>(&defaultValue), sizeof(float));
		}

		SpriteAnimator* spritePtr = m_pObjectManager->GetSpriteAnimator(i);
		if (spritePtr && spritePtr->IsValid())
		{
			outFile.write(reinterpret_cast<const char*>(spritePtr->GetLoop()), sizeof(bool));
		}
		else
		{
			bool defaultLoop = true;
			outFile.write(reinterpret_cast<const char*>(&defaultLoop), sizeof(bool));
		}
	}
}

AnimationClips& BaseEditScene::LoadAsset(wstring file)
{
	filesystem::path fullPath = m_folderPath / filesystem::path(file);
	auto extension = fullPath.extension();

	filesystem::path keyPath = fullPath;
	keyPath.replace_extension();

	wstring stringKey = keyPath.wstring();

	m_assetKey = stringKey;
	return m_pAssetManager->LoadAnimationClips(m_renderer, m_assetKey, fullPath);
}

void BaseEditScene::Finalize()
{
	pObjectManager.reset();
	m_pObjectManager = nullptr;

	m_fileList.clear();
	m_selectedFile.clear();
	m_folderPath.clear();
	m_assetKey.clear();
}