#include "GameScene.h"
#include "ModelManager.h"
#include "Player.h"
#include <cmath>

// 引数を受け取るように変更
void GameScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    // 受け取ったポインタをメンバ変数に保存
    object3dCommon_ = object3dCommon;


    // ==========================================
    // ★ 追加：フィールドの初期化
    // ==========================================
    ModelManager::GetInstance()->LoadModel("field.obj");

    field_ = new Object3d();
    field_->Initialize(object3dCommon_);
    field_->SetModel("field.obj");

    // チームメイトが設定していた角度と位置をそのままセット
    field_->SetRotate({ 85.0f, 0.0f, 0.0f });
    field_->SetTranslate({ 0.0f, 0.0f, 0.0f });

    // カメラの紐付けも忘れずに！
    field_->SetCamera(camera);
    // ==========================================


    boss_ = new Boss();
    if (boss_) {
        // ※ModelCommonは一旦 nullptr で渡しておきます
        boss_->Initialize(object3dCommon_, camera);
    }
}

void GameScene::Update(Player* player) {

    if (field_) {
        field_->Update();
    }

    // 3. ボスの更新処理（移動や攻撃ロジック）を呼ぶ
    if (boss_) {
        boss_->Update();
    }

    if (player && boss_) {
        if (boss_->IsShockwaveActive()) {
            // それぞれの座標を取得
            Vector3 pPos = player->GetTranslate();
            Vector3 wavePos = boss_->GetShockwavePos();
            Vector3 waveScale = boss_->GetShockwaveScale();

            // Z座標（奥行き）の距離を測る
            float diffZ = std::abs(pPos.z - wavePos.z);

            // Zが重なっていて(厚みの中に入っていて)、かつY(高さ)が波より低いか
            bool isHitZ = (diffZ < waveScale.z);
            bool isHitY = (pPos.y < waveScale.y);

            if (isHitZ && isHitY) {
                // コンソールに文字を出す！
                OutputDebugStringA("Hit Shockwave!!!\n");
            }
        }
    }

}

void GameScene::Draw() {

    if (field_) {
        field_->Draw();
    }

    // 4. ボスの描画処理を呼ぶ
    if (boss_) {
        boss_->Draw();
    }
}

// 忘れがちな後片付け
GameScene::~GameScene() {
    delete boss_;
    boss_ = nullptr;

    delete field_;
    field_ = nullptr;

}