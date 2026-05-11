#include "GameOverScene.h"
#include "SceneManager.h"
#include "Input.h"

// 引数を受け取るように変更
void GameOverScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    // 受け取ったポインタをメンバ変数に保存
    object3dCommon_ = object3dCommon;

}

void GameOverScene::Update(Player* player) {

    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {

        SceneManager::GetInstance()->ChangeScene("TITLE");

    }

}

void GameOverScene::Draw() {

}

void GameOverScene::Finalize()
{

}

// 忘れがちな後片付け
GameOverScene::~GameOverScene() {

}