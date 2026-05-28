#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "SpriteCommon.h" // 🌟忘れずに
#include <cassert>

// 1. 初期化：シーンが始まった時に1回だけ呼ばれる
void TitleScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    // メンバ変数に保存
    object3dCommon_ = object3dCommon;

    // SpriteCommonがセットされているか確認
    assert(spriteCommon_ != nullptr && "TitleSceneにSpriteCommonがセットされていません");

    // タイトル画像の読み込み（パスは自分の環境に合わせてください）
    std::string filePath = "resources/title.png";
    TextureManager::GetInstance()->LoadTexture(filePath);

    // スプライトの生成と初期化
    titleSprite_ = new Sprite();
    titleSprite_->Initialize(spriteCommon_, filePath);

    // 画面中央などに配置（1280x720の場合）
    titleSprite_->SetPosition({ 0.0f, 0.0f });
}

// 2. 更新：毎フレーム呼ばれる（計算や入力チェック）
void TitleScene::Update() {
    // スプライトの更新
    if (titleSprite_) {
        titleSprite_->Update();
    }

    // Enterキーが押されたらゲームシーンへ切り替え
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

// 3. 描画：毎フレーム呼ばれる（画面に出す処理）
void TitleScene::Draw() {
    // タイトル画像を描画
    if (titleSprite_) {
        titleSprite_->Draw();
    }
}

// 4. 終了処理：シーンが終わる時にメモリを逃がす
void TitleScene::Finalize() {
    if (titleSprite_) {
        delete titleSprite_;
        titleSprite_ = nullptr;
    }
}

// デストラクタ
TitleScene::~TitleScene() {
    Finalize();
}