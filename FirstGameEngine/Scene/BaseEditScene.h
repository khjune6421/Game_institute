#pragma once
#include "SceneManager.h"

class BaseEditScene : public Scene
{
	bool m_showScenePanel = false, m_showFolderPanel = false;
	wstring m_folderPath;
	wstring m_selectedFile;
	wstring m_assetKey;
	vector<wstring> m_fileList;
	float m_x = 0.0f, m_y = 0.0f, m_scaleX = 1.0f, m_scaleY = 1.0f, m_rotation = 0.0f;
	int clipIndex = 0;

public:
	void Initialize() override;

	void Update(float deltaTime) override;
	void Render() override;

	void RenderGUI();
	void ScenePanel(bool* pShowPanel);
	void FolderPanel(bool* pShowPanel);
	void BrowseFolder();
	void UpdateFileList();
	void SaveScene();
	AnimationClips& LoadAsset(wstring file);

	void Finalize() override;
};