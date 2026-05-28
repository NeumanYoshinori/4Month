#include "GameClearScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "SpriteCommon.h"
#include <cassert>

void GameClearScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    object3dCommon_ = object3dCommon;

    // SpriteCommonがセットされているかチェック
    assert(spriteCommon_ != nullptr && "GameClearSceneにSpriteCommonがセットされていません");

    // クリア画像の読み込み（ファイル名は自分の用意したものに合わせてください）
    std::string filePath = "resources/clear.png";
    TextureManager::GetInstance()->LoadTexture(filePath);

    // スプライトの生成と初期化
    clearSprite_ = new Sprite();
    clearSprite_->Initialize(spriteCommon_, filePath);
    clearSprite_->SetPosition({ 0.0f, 0.0f });
}

void GameClearScene::Update() {
    if (clearSprite_) {
        clearSprite_->Update();
    }

    // Enterキーでタイトルへ戻る
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

void GameClearScene::Draw() {
    if (clearSprite_) {
        clearSprite_->Draw();
    }
}

void GameClearScene::Finalize() {
    if (clearSprite_) {
        delete clearSprite_;
        clearSprite_ = nullptr;
    }
}

GameClearScene::~GameClearScene() {
    Finalize();
}