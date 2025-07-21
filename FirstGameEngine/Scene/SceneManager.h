#pragma once
#include <memory>
#include "ObjectManager.h"
#include "DirectX.h"
#include "AssetManager.h"

class ObjectManager;
namespace Renderer
{
	class DirectX;
}

using namespace std;

class Scene
{
protected:
	unique_ptr<ObjectManager> pObjectManager;
	ObjectManager* m_pObjectManager = nullptr;

	Renderer::DirectX* m_renderer = nullptr;
	AssetManager* m_pAssetManager = nullptr;

public:
	bool isSceneChanged = false;
	string changeSceneName;

	virtual void Initialize() = 0;

	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;

	virtual void Finalize() = 0;

	ObjectManager* GetObjectManager() { return pObjectManager.get(); }
	void SetAssetManager(AssetManager* assetManager) { m_pAssetManager = assetManager; }
	void SetRenderer(Renderer::DirectX* renderer) { m_renderer = renderer; }
};

class SceneManager
{
	shared_ptr<Scene> m_currentScene;
	Renderer::DirectX* m_renderer = nullptr;
	AssetManager m_assetManager;

	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	static SceneManager& GetInstance() { static SceneManager instance; return instance;	}

	void Update(float deltaTime);
	void Render();

	void SetRenderer(Renderer::DirectX* renderer) { m_renderer = renderer; }

	void ChangeScene(string sceneName);
	void SetScene(shared_ptr<Scene> scene);
	void EndScene(shared_ptr<Scene> scene);

	string sceneNames[2] = {};
	unordered_map<string, shared_ptr<Scene>> sceneMap;
};