#include "Skydome.h"
#include "ModelManager.h"

void Skydome::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    // 1. モデルの読み込み（Blenderで作った球体モデル）
    ModelManager::GetInstance()->LoadModel("skydome.obj");

    // 2. 3Dオブジェクトの生成と初期化
    object_ = new Object3d();
    object_->Initialize(object3dCommon);
    object_->SetModel("skydome.obj");
    object_->SetCamera(camera);

    // 3. サイズを巨大にする
    object_->SetScale({ kScale, kScale, kScale });
}

void Skydome::Update(Camera* camera) {
    if (!camera || !object_) return;

   
    //Vector3 camPos = camera->GetTranslate();
  //  object_->SetTranslate(camPos);

    object_->Update();
}

void Skydome::Draw() {
    if (object_) {
        object_->Draw();
    }
}

Skydome::~Skydome() {
    delete object_;
}