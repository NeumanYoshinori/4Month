#include "Boss.h"
#include "Object3dCommon.h"
#include "ModelManager.h" // ★これを超重要追加！
#include "Camera.h"
#include "Player.h" // ★ 追加：プレイヤーの情報を読み込む！
#include <cmath>    // ★ 追加：三平方の定理のルート計算に必要！


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
    objectLeftArm_->SetTranslate({ bossPos_.x - 0.5f, bossPos_.y, bossPos_.z });
    objectRightArm_->SetTranslate({ bossPos_.x + 0.5f, bossPos_.y, bossPos_.z });
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
    // plane.obj や cube.obj に戻す！
    ModelManager::GetInstance()->LoadModel("plane.obj");

    shockwave_ = new Object3d();
    shockwave_->Initialize(object3dCommon);
    shockwave_->SetModel("plane.obj");
    shockwave_->SetCamera(camera);
    // ※ 先ほど追加した shockwave_->SetRotate(...) は消して大丈夫です！

}

void Boss::Update(Player* player) {
    // ==========================================
    // 1. タイマーによる発射命令
    // ==========================================
    // ★ 修正：大技の最中（奥へ移動〜定位置に戻るまで）はタイマーを進めない！
    if (!isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_) {
        attackTimer_++;
    }

    // 左腕パンチ
    if (attackTimer_ == 60 && leftPunchState_ == PunchState::kIdle) {
        leftPunchState_ = PunchState::kPunch;

        // ★ 追加：発射の瞬間に1回だけ、プレイヤーの方向を計算して保存する！
        if (player) {
            Vector3 pPos = player->GetTranslate();
            Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z };

            float dx = targetPos.x - leftArmPos_.x;
            float dy = targetPos.y - leftArmPos_.y;
            float dz = targetPos.z - leftArmPos_.z;
            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            float speed = 0.5f; // ★ 一直線なので、少し速め（0.5fなど）にするとカッコいいです
            if (distance > 0.0f) {
                leftArmVelocity_.x = (dx / distance) * speed;
                leftArmVelocity_.y = (dy / distance) * speed;
                leftArmVelocity_.z = (dz / distance) * speed;
            }
        }

    }

    // 右腕パンチ（※念のため >= 120 ではなく == 120 にして事故を防ぎます）
    if (attackTimer_ == 120 && rightPunchState_ == PunchState::kIdle) {
        rightPunchState_ = PunchState::kPunch;

        // ★ 追加：右腕も同様に計算して保存する！
        if (player) {
            Vector3 pPos = player->GetTranslate();
            Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z };

            float dx = targetPos.x - rightArmPos_.x;
            float dy = targetPos.y - rightArmPos_.y;
            float dz = targetPos.z - rightArmPos_.z;
            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            float speed = 0.5f;
            if (distance > 0.0f) {
                rightArmVelocity_.x = (dx / distance) * speed;
                rightArmVelocity_.y = (dy / distance) * speed;
                rightArmVelocity_.z = (dz / distance) * speed;
            }
        }
    }
    // ③ 180フレーム（約3秒）でジャンプ開始！
    //if (attackTimer_ >= 180 && !isJumping_) {
    //    isJumping_ = true;
    //    velocityY_ = 0.75f; // ★ ジャンプ力（上に向かって飛ぶスピード）
    //}


    Vector3 leftShoulder = { bossPos_.x - 0.5f, bossPos_.y, bossPos_.z }; // 左肩の定位置

    switch (leftPunchState_) {
    case PunchState::kIdle:
        // 待機中は常に左肩の位置にピタッとくっついておく
        leftArmPos_ = leftShoulder;
        break;

    case PunchState::kPunch:
        //ここはホーミングにする場合
        //if (player) {
        //    Vector3 pPos = player->GetTranslate();
        //    Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z }; // プレイヤーのお腹を狙う

        //    // 三平方の定理で距離と方向を出す！
        //    float dx = targetPos.x - leftArmPos_.x;
        //    float dy = targetPos.y - leftArmPos_.y;
        //    float dz = targetPos.z - leftArmPos_.z;
        //    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        //    float speed = 0.1f; // ★ パンチの飛ぶスピード

        //    if (distance > 0.0f) {
        //        // 方向（1の長さ）にスピードを掛けて進む！
        //        leftArmPos_.x += (dx / distance) * speed;
        //        leftArmPos_.y += (dy / distance) * speed;
        //        leftArmPos_.z += (dz / distance) * speed;
        //    }
        //}

        //// タイマーを進めて、時間が来たら戻る
        //leftPunchTimer_++;
        //if (leftPunchTimer_ >= 120) { // ★ 120フレーム（約2秒）追いかけたら諦める
        //    leftPunchState_ = PunchState::kReturn;
        //    leftPunchTimer_ = 0; // タイマーリセット
        //}
        //break;

        // ★ 毎フレームの計算をやめて、保存しておいた速度を足すだけにする！
        leftArmPos_.x += leftArmVelocity_.x;
        leftArmPos_.y += leftArmVelocity_.y;
        leftArmPos_.z += leftArmVelocity_.z;

        leftPunchTimer_++;
        if (leftPunchTimer_ >= 120) {
            leftPunchState_ = PunchState::kReturn;
            leftPunchTimer_ = 0;
        }
        break;

    case PunchState::kReturn:
        // 肩に向かってホーミングして戻る！
        float dx = leftShoulder.x - leftArmPos_.x;
        float dy = leftShoulder.y - leftArmPos_.y;
        float dz = leftShoulder.z - leftArmPos_.z;
        float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        float returnSpeed = 0.4f; // 戻る時は少し速くする

        if (distance > returnSpeed) {
            leftArmPos_.x += (dx / distance) * returnSpeed;
            leftArmPos_.y += (dy / distance) * returnSpeed;
            leftArmPos_.z += (dz / distance) * returnSpeed;
        } else {
            leftArmPos_ = leftShoulder; // ピタッとくっつく
            leftPunchState_ = PunchState::kIdle; // 待機状態へ！
        }
        break;
    }

    // ==========================================
    // 3. 右腕の状態遷移（ホーミングロケットパンチ）
    // ==========================================
    Vector3 rightShoulder = { bossPos_.x + 0.5f, bossPos_.y, bossPos_.z }; // 右肩の定位置

    switch (rightPunchState_) {
    case PunchState::kIdle:
        rightArmPos_ = rightShoulder;
        break;

    case PunchState::kPunch:
        //ここはホーミングにする場合
        /*if (player) {
            Vector3 pPos = player->GetTranslate();
            Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z };

            float dx = targetPos.x - rightArmPos_.x;
            float dy = targetPos.y - rightArmPos_.y;
            float dz = targetPos.z - rightArmPos_.z;
            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            float speed = 0.1f;

            if (distance > 0.0f) {
                rightArmPos_.x += (dx / distance) * speed;
                rightArmPos_.y += (dy / distance) * speed;
                rightArmPos_.z += (dz / distance) * speed;
            }
        }

        rightPunchTimer_++;
        if (rightPunchTimer_ >= 120) {
            rightPunchState_ = PunchState::kReturn;
            rightPunchTimer_ = 0;
        }
        break;*/

        // ★ 右腕も保存しておいた速度を足すだけ！
        rightArmPos_.x += rightArmVelocity_.x;
        rightArmPos_.y += rightArmVelocity_.y;
        rightArmPos_.z += rightArmVelocity_.z;

        rightPunchTimer_++;
        if (rightPunchTimer_ >= 120) {
            rightPunchState_ = PunchState::kReturn;
            rightPunchTimer_ = 0;
        }
        break;

    case PunchState::kReturn:
        float dx = rightShoulder.x - rightArmPos_.x;
        float dy = rightShoulder.y - rightArmPos_.y;
        float dz = rightShoulder.z - rightArmPos_.z;
        float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        float returnSpeed = 0.4f;

        if (distance > returnSpeed) {
            rightArmPos_.x += (dx / distance) * returnSpeed;
            rightArmPos_.y += (dy / distance) * returnSpeed;
            rightArmPos_.z += (dz / distance) * returnSpeed;
        } else {
            rightArmPos_ = rightShoulder;
            rightPunchState_ = PunchState::kIdle;
        }
        break;
    }


    // ==========================================
      // 3. 大技：移動 → ジャンプ → 衝撃波 のコンボ
      // ==========================================
      // ① 180フレームで「奥への移動」を開始！
      // （※衝撃波がまだ画面に残っている間は、次の移動を始めないように待つ）
    if (attackTimer_ >= 180 && leftPunchState_ == PunchState::kIdle && rightPunchState_ == PunchState::kIdle && !isMovingToEdge_ && !isJumping_ && !isShockwaveActive_) {
        isMovingToEdge_ = true;
    }

    // ② 奥の端へスーーッと移動する処理
    if (isMovingToEdge_) {
        bossPos_.z += 0.2f; // ボスを奥（Zのプラス方向）へ後退させる

        // 画面の奥（例えば Z = 25.0f）に到達したら、移動をやめてジャンプ開始！
        if (bossPos_.z >= 15.0f) {
            bossPos_.z = 15.0f;
            isMovingToEdge_ = false;

            isJumping_ = true;
            velocityY_ = 0.6f; // ジャンプ力
        }
    }

    // ③ ジャンプと重力（着地）の処理
    if (isJumping_) {
        bossPos_.y += velocityY_;
        velocityY_ -= 0.05f;

        // 着地判定
        if (bossPos_.y <= 0.0f) {
            bossPos_.y = 0.0f;
            isJumping_ = false;
            attackTimer_ = 0; // タイマーリセット（またパンチから始まる）

            // ★着地した瞬間に、直線の衝撃波を発生させる！
            isShockwaveActive_ = true;

            // サイズ：横幅(X)をめちゃくちゃ広くして、画面全体を覆う「波」にする
            shockwaveScale_ = { 20.0f, 0.1f, 2.0f };

            // ボスの足元からスタート
            shockwavePos_ = { bossPos_.x, 0.01f, bossPos_.z };
        }
    }

    // ④ 直線衝撃波が「手前に迫ってくる」処理
    if (isShockwaveActive_) {
        // ※ 拡大（Scale）はもうさせず、位置（Z座標）だけを手前に動かす！
        shockwavePos_.z -= 0.1f; // 波が迫ってくるスピード

        // カメラの後ろ（例えば Z = -20.0f）まで波が通り過ぎたら消滅
        if (shockwavePos_.z < -20.0f) {
            isShockwaveActive_ = false;

            isReturningToCenter_ = true;
            // ※お好みで、攻撃が終わったらボスを元の位置（Z=10.0f）に戻すなどの処理を入れても面白いです！
        }

        shockwave_->SetTranslate(shockwavePos_);
        shockwave_->SetScale(shockwaveScale_);
        shockwave_->SetRotate({ 0.0f, 0.0f, 0.0f }); // 寝かせず、そのまま使う
        shockwave_->Update();
    }

    if (isReturningToCenter_) {
        bossPos_.z -= 0.2f; // 奥から手前（Zのマイナス方向）へ移動して戻る

        // 定位置（Z = 10.0f）に到着したら
        if (bossPos_.z <= 10.0f) {
            bossPos_.z = 10.0f;           // ピタッと止める
            isReturningToCenter_ = false; // 戻り状態を終了

            // ★ ここでタイマーをリセット！またパンチ攻撃から綺麗に再開します
            attackTimer_ = 0;
        }
    }

    // ==========================================
    // 4. 計算した結果を実際の3Dオブジェクトにセット
    // ==========================================
    float yOffset = 1.0f;   // 地面に埋まらないようにする「見た目の高さ」

    // ★修正：独立したXYZの座標をそのままセットする！
    objectBody_->SetTranslate({ bossPos_.x, bossPos_.y + yOffset, bossPos_.z });
    objectLeftArm_->SetTranslate({ leftArmPos_.x, leftArmPos_.y + yOffset, leftArmPos_.z });
    objectRightArm_->SetTranslate({ rightArmPos_.x, rightArmPos_.y + yOffset, rightArmPos_.z });
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