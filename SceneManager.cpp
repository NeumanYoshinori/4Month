#include "SceneManager.h"
#include "BaseScene.h"
#include "TitleScene.h"
#include "GameClearScene.h" // 追加
#include "GameOverScene.h"  // 追加
#include <cassert>

// インスタンスの実体を取得
SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize(Object3dCommon* object3dCommon, Camera* camera, SpriteCommon* spriteCommon)
{
	object3dCommon_ = object3dCommon;
	camera_ = camera;
	spriteCommon_ = spriteCommon;
}

void SceneManager::Update(Player* player)
{
    if (nextScene_) {
        /*if (scene_) {
            scene_->Finalize();
         
            delete scene_;
        }*/
        scene_ = nextScene_;
        nextScene_ = nullptr;

        // ここで TitleScene かどうか判定して、SpriteCommon を渡す
        TitleScene* titleScene = dynamic_cast<TitleScene*>(scene_);
        if (titleScene) {
            titleScene->SetSpriteCommon(spriteCommon_);
        }

        // ゲームクリアシーンへの受け渡しを追加
        GameClearScene* clearScene = dynamic_cast<GameClearScene*>(scene_);
        if (clearScene) {
            clearScene->SetSpriteCommon(spriteCommon_);
        }

        // ゲームオーバーシーンへの受け渡しを追加
        GameOverScene* overScene = dynamic_cast<GameOverScene*>(scene_);
        if (overScene) {
            overScene->SetSpriteCommon(spriteCommon_);
        }

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