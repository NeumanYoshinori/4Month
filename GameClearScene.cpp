#include "GameClearScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "SpriteCommon.h"
#include <cassert>
#include "ModelManager.h"

void GameClearScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    object3dCommon_ = object3dCommon;

    camera_ = camera;

    // モデル読み込み
    ModelManager::GetInstance()->LoadModel("3DClear.obj"); // クリア用3D文字
    ModelManager::GetInstance()->LoadModel("SkyDome.obj");
    ModelManager::GetInstance()->LoadModel("3DEnter.obj");

    // 天球の初期化
    skydome_ = new Skydome();
    skydome_->Initialize(object3dCommon_, camera_);

    // 3D文字の初期化
    clearTextObject_ = new Object3d();
    clearTextObject_->Initialize(object3dCommon_);
    clearTextObject_->SetModel("3DClear.obj");
    clearTextObject_->SetCamera(camera_);

    EnterTextObject_ = new Object3d();
    EnterTextObject_->Initialize(object3dCommon_);
    EnterTextObject_->SetModel("3DEnter.obj");
    EnterTextObject_->SetCamera(camera_);

    // 配置の設定
    clearTextObject_->SetTranslate({ 0.0f, -3.0f, 25.0f });
    clearTextObject_->SetScale({ 2.0f, 2.0f, 2.0f });

    EnterTextObject_->SetTranslate({ 0.0f,-5.0f, 25.0f });
    EnterTextObject_->SetScale({ 1.0f, 1.0f, 1.0f });


    // カメラリセット（天球の中心へ）
    camera_->SetTranslate({ 0.0f, 0.0f, 0.0f });
    camera_->SetRotate({ 0.0f, 0.0f, 0.0f });


    //// SpriteCommonがセットされているかチェック
    //assert(spriteCommon_ != nullptr && "GameClearSceneにSpriteCommonがセットされていません");

    //// クリア画像の読み込み（ファイル名は自分の用意したものに合わせてください）
    //std::string filePath = "resources/clear.png";
    //TextureManager::GetInstance()->LoadTexture(filePath);

    //// スプライトの生成と初期化
    //clearSprite_ = new Sprite();
    //clearSprite_->Initialize(spriteCommon_, filePath);
    //clearSprite_->SetPosition({ 0.0f, 0.0f });
}

void GameClearScene::Update(Player* player) {
    if (skydome_) {
        skydome_->Update(camera_);
    }

 
    if (clearTextObject_) {
        static float timer = 0.0f;
        timer += 0.05f; // 跳ねるスピード

        // 現状の座標を取得
        Vector3 pos = clearTextObject_->GetTranslate();

        // Y座標（高さ）を std::abs(std::sin) で地面に弾む挙動にする
        pos.y = std::abs(std::sin(timer)) * 2.0f - 2.0f;

        pos.z = 25.0f;

        clearTextObject_->SetTranslate(pos);
    
        clearTextObject_->SetRotate({ 0.0f, 0.0f, 0.0f });

        clearTextObject_->Update();
    }


    if (EnterTextObject_) {
        EnterTextObject_->Update();
    }

    // Enterキーでタイトルへ戻る
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

void GameClearScene::Draw() {
   /* if (clearSprite_) {
        clearSprite_->Draw();
    }*/

    if (skydome_) skydome_->Draw();
    if (clearTextObject_) clearTextObject_->Draw();
    if (EnterTextObject_) EnterTextObject_->Draw();

}

void GameClearScene::Finalize() {
    /*if (clearSprite_) {
        delete clearSprite_;
        clearSprite_ = nullptr;
    }*/

    if (clearTextObject_) {
        delete clearTextObject_;
        clearTextObject_ = nullptr;
    }

    if (EnterTextObject_) {
        delete EnterTextObject_;
        EnterTextObject_ = nullptr;
    }

    if (skydome_) {
        delete skydome_;
        skydome_ = nullptr;
    }

}

GameClearScene::~GameClearScene() {
    //Finalize();
}