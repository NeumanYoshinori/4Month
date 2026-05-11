#include "GameClearScene.h"
#include "SceneManager.h"
#include "Input.h"

// 引数を受け取るように変更
void GameClearScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    // 受け取ったポインタをメンバ変数に保存
    object3dCommon_ = object3dCommon;

}

void GameClearScene::Update(Player* player) {

    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {

        SceneManager::GetInstance()->ChangeScene("TITLE");

    }

}

void GameClearScene::Draw() {

}

void GameClearScene::Finalize()
{

}

// 忘れがちな後片付け
GameClearScene::~GameClearScene() {

}