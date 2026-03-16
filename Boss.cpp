#include "Boss.h"
#include "Object3dCommon.h"
#include "ModelManager.h" // ★これを超重要追加！
#include "Camera.h"

// 引数から ModelCommon を消してスッキリさせました
void Boss::Initialize(Object3dCommon* object3dCommon, Camera* camera) {

    // ==========================================
    // 1. 胴体（Body）の準備
    // ==========================================
    // ① マネージャーに名前を教えて読み込んでもらうだけ！
    ModelManager::GetInstance()->LoadModel("alphaBoss.obj");

    objectBody_ = new Object3d();
    objectBody_->Initialize(object3dCommon);
    // ② オブジェクトに文字列を渡すだけでセット完了！
    objectBody_->SetModel("alphaBoss.obj");


    // ==========================================
    // 2. 左腕（Left Arm）の準備
    // ==========================================
    ModelManager::GetInstance()->LoadModel("alphaBossLeftArm.obj");

    objectLeftArm_ = new Object3d();
    objectLeftArm_->Initialize(object3dCommon);
    objectLeftArm_->SetModel("alphaBossLeftArm.obj");


    // ==========================================
    // 3. 右腕（Right Arm）の準備
    // ==========================================
    ModelManager::GetInstance()->LoadModel("alphaBossRightArm.obj");

    objectRightArm_ = new Object3d();
    objectRightArm_->Initialize(object3dCommon);
    objectRightArm_->SetModel("alphaBossRightArm.obj");


    // ==========================================
     // 4. サイズと初期位置の設定
     // ==========================================
     // ① サイズを小さくする（0.1倍など、ちょうどいいサイズを探します）
    Vector3 bossScale = { 0.1f, 0.1f, 0.1f }; // ★ここを 0.5f や 0.05f などに変えて調整
    objectBody_->SetScale(bossScale);
    objectLeftArm_->SetScale(bossScale);
    objectRightArm_->SetScale(bossScale);

    // ② カメラから少し離れた位置（奥）に置く
    // 今カメラが Z: -10.0f にいるので、ボスを Z: 10.0f くらいに置くと全体が見えやすいです
    Vector3 bossPos = { 0.0f,-1.57f, 10.0f };
    objectBody_->SetTranslate(bossPos_);
    objectLeftArm_->SetTranslate({ bossPos_.x - 2.0f, bossPos_.y, bossPos_.z });
    objectRightArm_->SetTranslate({ bossPos_.x + 2.0f, bossPos_.y, bossPos_.z });
    // ==========================================
    // 5. 向き（回転）の設定
    // ==============================,============
    // { X軸の回転, Y軸の回転, Z軸の回転 } です。
    // コマみたいに横を向かせたい（旋回させたい）場合は、真ん中の「Y」の値をいじります。

    // 例：180度回して反対を向かせる
    Vector3 bossRotate = { 0.0f, 1.57f, 0.0f }; 
    objectBody_->SetRotate(bossRotate);
    objectLeftArm_->SetRotate(bossRotate);
    objectRightArm_->SetRotate(bossRotate);

    float armOffset = 2.0f;

    // 左腕はマイナス方向、右腕はプラス方向にズラす
  // もし前後（胸と背中）に腕がいってしまった場合の書き方
    Vector3 leftArmPos = { bossPos_.x - armOffset, bossPos_.y, bossPos_.z };
    Vector3 rightArmPos = { bossPos_.x + armOffset, bossPos_.y, bossPos_.z };

    objectLeftArm_->SetTranslate(leftArmPos);
    objectRightArm_->SetTranslate(rightArmPos);

    objectBody_->SetCamera(camera);
    objectLeftArm_->SetCamera(camera);
    objectRightArm_->SetCamera(camera);


    // ==========================================
    // 6. 衝撃波の準備
    // ==========================================
    ModelManager::GetInstance()->LoadModel("plane.obj");

    shockwave_ = new Object3d();
    shockwave_->Initialize(object3dCommon);
    shockwave_->SetModel("plane.obj");
    shockwave_->SetCamera(camera);

}

void Boss::Update() {
    // ==========================================
    // 1. タイマーによる発射命令
    // ==========================================
    attackTimer_++;

    // タイマーが60（約1秒）になったら、左腕が待機中の場合のみ発射！
    if (attackTimer_ == 60 && leftPunchState_ == PunchState::kIdle) {
        leftPunchState_ = PunchState::kPunch;
        
    }
    // タイマーが120（約2秒）になったら、右腕が待機中の場合のみ発射！
    if (attackTimer_ >= 120 && rightPunchState_ == PunchState::kIdle) {
        rightPunchState_ = PunchState::kPunch;
      //  attackTimer_ = 0; // ここでタイマーをリセットして、また左腕のターンへ
    }
    // ③ 180フレーム（約3秒）でジャンプ開始！
    if (attackTimer_ >= 180 && !isJumping_) {
        isJumping_ = true;
        velocityY_ = 0.75f; // ★ ジャンプ力（上に向かって飛ぶスピード）
    }


    // ==========================================
    // 2. 左腕の状態遷移（ロケットパンチの動き）
    // ==========================================
    switch (leftPunchState_) {
    case PunchState::kIdle:
        break; // 待機中は動かさない
    case PunchState::kPunch:
        leftArmZ_ -= 0.1f; // 手前に飛ばす
        if (leftArmZ_ < -6.0f) {
            leftPunchState_ = PunchState::kReturn; // 限界まで飛んだら戻る
        }
        break;
    case PunchState::kReturn:
        leftArmZ_ += 0.2f; // 元の位置へ戻る
        if (leftArmZ_ >= 0.0f) {
            leftArmZ_ = 0.0f;
            leftPunchState_ = PunchState::kIdle; // 戻りきったら待機状態へ
        }
        break;
    }

    // ==========================================
    // 3. 右腕の状態遷移（ロケットパンチの動き）
    // ==========================================
    switch (rightPunchState_) {
    case PunchState::kIdle:
        break; // 待機中は動かさない
    case PunchState::kPunch:
        rightArmZ_ -= 0.1f;
        if (rightArmZ_ < -6.0f) {
            rightPunchState_ = PunchState::kReturn;
        }
        break;
    case PunchState::kReturn:
        rightArmZ_ += 0.2f;
        if (rightArmZ_ >= 0.0f) {
            rightArmZ_ = 0.0f;
            rightPunchState_ = PunchState::kIdle;
        }
        break;
    }


    // ==========================================
    // ★ 追加：ジャンプと重力（着地）の物理演算
    // ==========================================
    if (isJumping_) {
        // スピードの分だけ高さを変える
        bossPos_.y += velocityY_;

        // 重力をかける（毎フレーム少しずつ下向きの力を足す）
        velocityY_ -= 0.05f;

        // 着地判定（フィールドの高さ Y = 0.0f にぶつかったら）
        if (bossPos_.y <= 0.0f) {
            bossPos_.y = 0.0f;      // 地面にめり込まないようにピタッと止める
            isJumping_ = false;     // ジャンプ終了
            attackTimer_ = 0;       // タイマーをリセットして、また左パンチから繰り返す

            // ⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎⬇︎
            
            // ==========================================
            // ★ 追加：着地した瞬間に衝撃波を発生させる！
            // ==========================================
            isShockwaveActive_ = true;
            shockwaveScale_ = { 0.1f, 0.1f, 0.1f }; // 初期サイズは小さく

            // ボスの足元にセット（地面と重なってチカチカするバグを防ぐため、Yを 0.01f だけ浮かせる）
            shockwavePos_ = { bossPos_.x, 0.01f, bossPos_.z };

            // ⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎⬆︎
        }
    }

    // ==========================================
    // 4. 計算した結果を実際の3Dオブジェクトにセット
    // ==========================================
    float armOffset = 0.5f; // 脇への距離

    objectBody_->SetTranslate(bossPos_);

    // ★ 左腕も Z軸 に leftArmZ_ を足すように変更！
    objectLeftArm_->SetTranslate({ bossPos_.x - armOffset, bossPos_.y, bossPos_.z + leftArmZ_ });
    objectRightArm_->SetTranslate({ bossPos_.x + armOffset, bossPos_.y, bossPos_.z + rightArmZ_ });

    // 行列更新
    objectBody_->Update();
    objectLeftArm_->Update();
    objectRightArm_->Update();
}
void Boss::Draw() {
    // 描画
    if (objectBody_) { objectBody_->Draw(); }
    if (objectLeftArm_) { objectLeftArm_->Draw(); }
    if (objectRightArm_) { objectRightArm_->Draw(); }

    if (isShockwaveActive_ && shockwave_) {
        shockwave_->Draw();
    }
}

Boss::~Boss() {
    delete objectBody_;
    delete objectLeftArm_;
    delete objectRightArm_;
    delete shockwave_;
}

//
//次はボスの攻撃（ロケットパンチ）
//