#include "SceneManager.h"

void SceneManager::Update(float deltaTime)
{
	if (m_currentScene)
	{
		m_currentScene->Update(deltaTime);
	}

	if (m_currentScene && m_currentScene->isSceneChanged)
	{
		m_currentScene->isSceneChanged = false;
		ChangeScene(m_currentScene->changeSceneName);
	}
}

void SceneManager::Render()
{
	if (m_currentScene)
	{
		m_currentScene->Render();
	}
}

void SceneManager::ChangeScene(string sceneName)
{
	auto it = sceneMap.find(sceneName);
	if (it != sceneMap.end())
	{
		m_currentScene = it->second;
	}
}

void SceneManager::SetScene(shared_ptr<Scene> scene)
{
	if (scene)
	{
		scene->SetAssetManager(&m_assetManager);
		scene->SetRenderer(m_renderer);
		scene->Initialize();
	}
}

void SceneManager::EndScene(shared_ptr<Scene> scene)
{
	if (scene)
	{
		scene->Finalize();
		scene.reset();
	}
}