#pragma once

#include "AbstractSceneFactory.h"
#include <string>

class BaseScene;
class WinApp;
class GraphicsDevice;

class SceneManager
{
private:
	// シングルトン化のためのプライベート化
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

public:
	void Initialize(Object3dCommon* object3dCommon, Camera* camera);
	void ChangeScene(const std::string& sceneName);
	void Update(Player* player);
	void Draw();
	static SceneManager* GetInstance();
	void Finalize();

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
	BaseScene* scene_ = nullptr;
	BaseScene* nextScene_ = nullptr;

	Object3dCommon* object3dCommon_ = nullptr;
	Camera* camera_ = nullptr;

	AbstractSceneFactory* sceneFactory_ = nullptr;
};