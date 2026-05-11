#include "SceneManager.h"
#include "BaseScene.h"
#include <cassert>

// インスタンスの実体を取得
SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize(Object3dCommon* object3dCommon, Camera* camera)
{
	object3dCommon_ = object3dCommon;
	camera_ = camera;
}

void SceneManager::Update(Player* player)
{
	if (nextScene_) {
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}
		scene_ = nextScene_;
		nextScene_ = nullptr;

		scene_->Initialize(object3dCommon_, camera_);
	}
	if (scene_) {
		scene_->Update(player);
	}
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::Draw()
{
	if (scene_) {
		scene_->Draw();
	}
}

void SceneManager::Finalize()
{
	if (scene_) {
		scene_->Finalize();
		delete scene_;
		scene_ = nullptr;
	}
}