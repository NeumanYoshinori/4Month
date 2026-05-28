#include "Rule.h"
#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "SpriteCommon.h"
#include <cassert>
#include "ModelManager.h"

void RuleScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    object3dCommon_ = object3dCommon;

    camera_ = camera;

    // モデル読み込み
    ModelManager::GetInstance()->LoadModel("3DRule.obj"); //3D文字
    ModelManager::GetInstance()->LoadModel("SkyDome.obj");
    ModelManager::GetInstance()->LoadModel("3DEnter.obj");

    // 天球の初期化
    skydome_ = new Skydome();
    skydome_->Initialize(object3dCommon_, camera_);

    // 3D文字の初期化
    RuleTextObject_ = new Object3d();
    RuleTextObject_->Initialize(object3dCommon_);
    RuleTextObject_->SetModel("3DRule.obj");
    RuleTextObject_->SetCamera(camera_);

    EnterTextObject_ = new Object3d();
    EnterTextObject_->Initialize(object3dCommon_);
    EnterTextObject_->SetModel("3DEnter.obj");
    EnterTextObject_->SetCamera(camera_);

    // 配置の設定
    RuleTextObject_->SetTranslate({ 0.0f, -3.0f, 25.0f });
    RuleTextObject_->SetScale({ 0.5f, 0.5f, 0.5f });

    EnterTextObject_->SetTranslate({ 0.0f,-5.0f, 25.0f });
    EnterTextObject_->SetScale({ 1.0f, 1.0f, 1.0f });


    // カメラリセット（天球の中心へ）
    camera_->SetTranslate({ 0.0f, 0.0f, 0.0f });
    camera_->SetRotate({ 0.0f, 0.0f, 0.0f });

}

void RuleScene::Update(Player* player) {
    if (skydome_) {
        skydome_->Update(camera_);
    }


    if (RuleTextObject_) {
     
        static float timer = 0.0f;
     
        timer += 0.02f;

        // 現状の座標を取得
        Vector3 pos = RuleTextObject_->GetTranslate();

                
        pos.y = std::sin(timer) * 0.3f - 2.0f;

        pos.z = 25.0f;

        RuleTextObject_->SetTranslate(pos);

        // 角度は正面で固定して読みにくさを解消
        RuleTextObject_->SetRotate({ 0.0f, 0.0f, 0.0f });

        RuleTextObject_->Update();
    }


    if (EnterTextObject_) {
        EnterTextObject_->Update();
    }

    // Enterキーでタイトルへ戻る
    if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

void RuleScene::Draw() {
  

    if (skydome_) skydome_->Draw();
    if (RuleTextObject_) RuleTextObject_->Draw();
   // if (EnterTextObject_) EnterTextObject_->Draw();

}

void RuleScene::Finalize() {
  

    if (RuleTextObject_) {
        delete RuleTextObject_;
        RuleTextObject_ = nullptr;
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

RuleScene::~RuleScene() {
    //Finalize();
}