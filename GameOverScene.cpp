#include "GameOverScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "SpriteCommon.h"
#include <cassert>
#include "ModelManager.h"



void GameOverScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    object3dCommon_ = object3dCommon;
    camera_ = camera;

    // モデル読み込み
    ModelManager::GetInstance()->LoadModel("3DGameover.obj");
    ModelManager::GetInstance()->LoadModel("SkyDome.obj");
    ModelManager::GetInstance()->LoadModel("3DEnter.obj");

    // 天球の初期化
    skydome_ = new Skydome();
    skydome_->Initialize(object3dCommon_, camera_);

    // 3D文字の初期化
    gameOverTextObject_ = new Object3d();
    gameOverTextObject_->Initialize(object3dCommon_);
    gameOverTextObject_->SetModel("3DGameover.obj");
    gameOverTextObject_->SetCamera(camera_);

    EnterTextObject_ = new Object3d();
    EnterTextObject_->Initialize(object3dCommon_);
    EnterTextObject_->SetModel("3DEnter.obj");
    EnterTextObject_->SetCamera(camera_);

    // 配置の設定
    gameOverTextObject_->SetTranslate({ 0.0f,-2.0f, 25.0f });
    gameOverTextObject_->SetScale({ 2.0f, 2.0f, 2.0f });

    EnterTextObject_->SetTranslate({ 0.0f,-5.0f, 25.0f });
    EnterTextObject_->SetScale({ 1.0f, 1.0f, 1.0f });

    // カメラリセット
    camera_->SetTranslate({ 0.0f, 0.0f, 0.0f });
    camera_->SetRotate({ 0.0f, 0.0f, 0.0f });

    


    //// SpriteCommonがセットされているかチェック
    //assert(spriteCommon_ != nullptr && "GameOverSceneにSpriteCommonがセットされていません");

    //// ゲームオーバー画像の読み込み
    //std::string filePath = "resources/gameover.png";
    //TextureManager::GetInstance()->LoadTexture(filePath);

    //// スプライトの生成と初期化
    //gameOverSprite_ = new Sprite();
    //gameOverSprite_->Initialize(spriteCommon_, filePath);
    //gameOverSprite_->SetPosition({ 0.0f, 0.0f });
}

void GameOverScene::Update(Player* player) {
    skydome_->Update(camera_);

    // 少し左右に揺れる演出
    static float timer = 0.0f;
    timer += 0.03f;
    Vector3 rot = gameOverTextObject_->GetRotate();
    rot.z = std::sin(timer) * 0.1f;
    gameOverTextObject_->SetRotate(rot);

    gameOverTextObject_->Update();
    
    EnterTextObject_->Update();

    /*if (gameOverSprite_) {
        gameOverSprite_->Update();
    }*/

    // Enterキーでタイトルへ戻る
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

void GameOverScene::Draw() {
    if (skydome_) skydome_->Draw();
    if (gameOverTextObject_) gameOverTextObject_->Draw();
    if (EnterTextObject_) EnterTextObject_->Draw();

    /*if (gameOverSprite_) {
        gameOverSprite_->Draw();
    }*/
}

void GameOverScene::Finalize() {
    // 🌟 nullptr チェックをしてから delete し、即座に nullptr を代入する
    if (gameOverTextObject_) {
        delete gameOverTextObject_;
        gameOverTextObject_ = nullptr;
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

GameOverScene::~GameOverScene() {
    //Finalize();
}