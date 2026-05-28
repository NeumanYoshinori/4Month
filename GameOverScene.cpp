#include "GameOverScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "SpriteCommon.h"
#include <cassert>

void GameOverScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    object3dCommon_ = object3dCommon;

    // SpriteCommonがセットされているかチェック
    assert(spriteCommon_ != nullptr && "GameOverSceneにSpriteCommonがセットされていません");

    // ゲームオーバー画像の読み込み
    std::string filePath = "resources/gameover.png";
    TextureManager::GetInstance()->LoadTexture(filePath);

    // スプライトの生成と初期化
    gameOverSprite_ = new Sprite();
    gameOverSprite_->Initialize(spriteCommon_, filePath);
    gameOverSprite_->SetPosition({ 0.0f, 0.0f });
}

void GameOverScene::Update(Player* player) {
    if (gameOverSprite_) {
        gameOverSprite_->Update();
    }

    // Enterキーでタイトルへ戻る
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

void GameOverScene::Draw() {
    if (gameOverSprite_) {
        gameOverSprite_->Draw();
    }
}

void GameOverScene::Finalize() {
    if (gameOverSprite_) {
        delete gameOverSprite_;
        gameOverSprite_ = nullptr;
    }
}

GameOverScene::~GameOverScene() {
    Finalize();
}