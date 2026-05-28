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
    camera_ = camera;




    //モデル読み込み
    ModelManager::GetInstance()->LoadModel("3DTitle.obj");
    ModelManager::GetInstance()->LoadModel("SkyDome.obj");
    ModelManager::GetInstance()->LoadModel("3DEnter.obj");


    skydome_ = new Skydome();
    skydome_->Initialize(object3dCommon_, camera_);

    //初期化
    // --- 3Dタイトル文字の初期化 ---
    titleTextObject_ = new Object3d();
    titleTextObject_->Initialize(object3dCommon_);
    titleTextObject_->SetModel("3DTitle.obj");
    titleTextObject_->SetCamera(camera_);

    // 配置の設定（画面の中央、少し奥に置く）
    titleTextObject_->SetTranslate({ 0.5f, 0.0f, 0.0f });
    titleTextObject_->SetScale({ 2.0f, 2.0f, 2.0f });
    titleTextObject_->SetRotate({ 0.0f, 0.0f, 0.0f });


    EnterTextObject_ = new Object3d();
    EnterTextObject_->Initialize(object3dCommon_);
    EnterTextObject_->SetModel("3DEnter.obj");
    EnterTextObject_->SetCamera(camera_);

    EnterTextObject_->SetTranslate({ 0.0f,-5.0f, 25.0f });
    EnterTextObject_->SetScale({ 1.0f, 1.0f, 1.0f });

    // タイトル用のカメラ位置に調整（必要なら）
    camera_->SetTranslate({ 0.0f, 1.5f, -20.0f });
    camera_->SetRotate({ 0.0f, 0.0f, 0.0f });


    // SpriteCommonがセットされているか確認
    //assert(spriteCommon_ != nullptr && "TitleSceneにSpriteCommonがセットされていません");

    // タイトル画像の読み込み（パスは自分の環境に合わせてください）
    /*std::string filePath = "resources/title.png";
    TextureManager::GetInstance()->LoadTexture(filePath);*/

    //// スプライトの生成と初期化
    //titleSprite_ = new Sprite();
    //titleSprite_->Initialize(spriteCommon_, filePath);

    //// 画面中央などに配置（1280x720の場合）
    //titleSprite_->SetPosition({ 0.0f, 0.0f });
}

// 2. 更新：毎フレーム呼ばれる（計算や入力チェック）
void TitleScene::Update(Player* player) {
    
    // 天空の更新
    skydome_->Update(camera_);

    // 3D文字に「浮遊感」を出す演出
    static float timer = 0.0f;
    timer += 0.05f;
    Vector3 pos = titleTextObject_->GetTranslate();
    pos.y = std::sin(timer) * 0.1f; // 上下にふわふわさせる
    titleTextObject_->SetTranslate(pos);

    titleTextObject_->Update();
    
    EnterTextObject_->Update();

    //// スプライトの更新
    //if (titleSprite_) {
    //    titleSprite_->Update();
    //}

    // Enterキーが押されたらゲームシーンへ切り替え
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("RULE");
    }
}

// 3. 描画：毎フレーム呼ばれる（画面に出す処理）
void TitleScene::Draw() {
    // 背景（天空）を先に描画
    if (skydome_) {
        skydome_->Draw();
    }

    // 3D文字を描画
    if (titleTextObject_) {
        titleTextObject_->Draw();
    }
    
    if (EnterTextObject_) {
        EnterTextObject_->Draw();
    }

    // タイトル画像を描画
   /* if (titleSprite_) {
        titleSprite_->Draw();
    }*/
}

// 4. 終了処理：シーンが終わる時にメモリを逃がす
void TitleScene::Finalize() {
    delete titleTextObject_;
    titleTextObject_ = nullptr;

    delete skydome_;
    skydome_ = nullptr;
    
    delete EnterTextObject_;
    EnterTextObject_ = nullptr;

    
    /* if (titleSprite_) {
        delete titleSprite_;
        titleSprite_ = nullptr;
    }*/
}

// デストラクタ
TitleScene::~TitleScene() {
  //  Finalize();
}