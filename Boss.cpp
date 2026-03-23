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


    // ==========================================, ,
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
    ModelManager::GetInstance()->LoadModel("wave.obj");

    shockwave_ = new Object3d();
    shockwave_->Initialize(object3dCommon);
    shockwave_->SetModel("wave.obj");
    shockwave_->SetCamera(camera);
    // ※ 先ほど追加した shockwave_->SetRotate(...) は消して大丈夫です！

    // ==========================================
    // 7. ★追加：ミサイルの準備
    // ==========================================
    // ※とりあえず既存の plane.obj を使いますが、後で missile.obj などに変えられます！
    ModelManager::GetInstance()->LoadModel("missile.obj");

    for (int i = 0; i < kMaxMissiles; i++) {
        missiles_[i] = new Object3d();
        missiles_[i]->Initialize(object3dCommon);
        missiles_[i]->SetModel("missile.obj");
        missiles_[i]->SetCamera(camera);
    }

    // ==========================================
    // 8. ★ 追加：爆発（範囲攻撃）の準備
    // ==========================================
    ModelManager::GetInstance()->LoadModel("explosion.obj");

    explosion_ = new Object3d();
    explosion_->Initialize(object3dCommon);
    explosion_->SetModel("explosion.obj"); // 波紋のように広がる円（平面）
    explosion_->SetCamera(camera);

}

void Boss::Update(Player* player) {

    // ==========================================
    // ① 形態変化（第1 → 第2）の演出中！
    // ==========================================
    if (isTransitioning_) {
        transitionTimer_++;

        // 腕を強制的に定位置に戻して待機
        leftArmPos_ = { bossPos_.x - 0.5f, bossPos_.y, bossPos_.z };
        rightArmPos_ = { bossPos_.x + 0.5f, bossPos_.y, bossPos_.z };
        leftPunchState_ = PunchState::kIdle;
        rightPunchState_ = PunchState::kIdle;

        // 120フレーム（2秒）経ったら第2形態スタート！
        if (transitionTimer_ >= 120) {
            phase_ = 2;              // 第2形態へ！
            hp_ = 100;               // ★ 第2形態のHP！
            attackTimer_ = 0;        // 攻撃タイマーリセット
            isTransitioning_ = false;// 演出終了
            jumpCount_ = 0;          // ジャンプ回数リセット
            OutputDebugStringA("BOSS PHASE 2 START!!!\n");
        }
    }
    // ==========================================
    // ② 第1形態の動き（今までの自機狙いロケットパンチ）
    // ==========================================
    else if (phase_ == 1) {
        if (!isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_) {
            attackTimer_++;
        }

        // 左腕パンチ
        if (attackTimer_ == 60 && leftPunchState_ == PunchState::kIdle) {
            leftPunchState_ = PunchState::kPunch;
            if (player) {
                Vector3 pPos = player->GetTranslate();
                Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z };
                float dx = targetPos.x - leftArmPos_.x; float dy = targetPos.y - leftArmPos_.y; float dz = targetPos.z - leftArmPos_.z;
                float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
                float speed = 0.5f;
                if (distance > 0.0f) {
                    leftArmVelocity_ = { (dx / distance) * speed, (dy / distance) * speed, (dz / distance) * speed };
                }
            }
        }
        // 右腕パンチ
        if (attackTimer_ == 120 && rightPunchState_ == PunchState::kIdle) {
            rightPunchState_ = PunchState::kPunch;
            if (player) {
                Vector3 pPos = player->GetTranslate();
                Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z };
                float dx = targetPos.x - rightArmPos_.x; float dy = targetPos.y - rightArmPos_.y; float dz = targetPos.z - rightArmPos_.z;
                float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
                float speed = 0.5f;
                if (distance > 0.0f) {
                    rightArmVelocity_ = { (dx / distance) * speed, (dy / distance) * speed, (dz / distance) * speed };
                }
            }
        }
        // 大技（ジャンプ＆衝撃波）
        if (attackTimer_ >= 180 && leftPunchState_ == PunchState::kIdle && rightPunchState_ == PunchState::kIdle && !isMovingToEdge_ && !isJumping_ && !isShockwaveActive_) {
            isMovingToEdge_ = true;
        }
    }
    // ==========================================
    // ③ 第2形態の動き（暴走モード：連続ジャンプ衝撃波！）
    // ==========================================
    else if (phase_ == 2) {
        // 攻撃のテンポを管理
        if (!isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_) {
            attackTimer_++;
        }

        // ==========================================
        // ★ 攻撃0：ホーミングミサイル発射！
        // ==========================================
        if (attackTimer_ == 10) { // ジャンプ攻撃の少し前に撃つ！
            for (int i = 0; i < kMaxMissiles; i++) {
                isMissileActive_[i] = true;
                missileHomingTimer_[i] = 75; // ★ 90フレーム（1.5秒間）だけホーミングする！

                // 発射位置：ボスの少し上、左右に振り分ける
                float offsetX = (i == 0) ? -2.0f : 2.0f;
                missilePos_[i] = { bossPos_.x + offsetX, bossPos_.y + 3.0f, bossPos_.z };

                // 最初の初速：上にフワッと撃ち上げる（マクロス風！）
                missileVelocity_[i] = { 0.0f, 0.3f, -0.1f };
            }
        }

        // 攻撃1：連続ジャンプ衝撃波（3連続）
        if (attackTimer_ == 60 && !isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_) {
            isMovingToEdge_ = true;
            jumpCount_ = 0; // ジャンプ回数をリセット
        }

        // ==========================================
        // ★ 攻撃2：ブラックホール（プレイヤーを吸引）
        // ==========================================
        // タイマーが150〜330の間（約3秒間）、強烈に吸い寄せる！
        if (attackTimer_ >= 150 && attackTimer_ < 330) {
            isSuctionActive_ = true;

            if (player) {
                Vector3 pPos = player->GetTranslate();
                float dx = bossPos_.x - pPos.x;
                float dz = bossPos_.z - pPos.z; // 足元に吸い寄せるのでY（高さ）は無視
                float dist = std::sqrt(dx * dx + dz * dz);

                if (dist > 0.1f) {
                    // ★ プレイヤーの逃げる速度（0.1f）より少しだけ遅い力（0.08f）で引っ張る
                    // これにより「必死にSキーで走ればギリギリ逃げられる」絶妙なバランスになります！
                    float suctionPower = 0.08f;
                    pPos.x += (dx / dist) * suctionPower;
                    pPos.z += (dz / dist) * suctionPower;

                    // ボス側からプレイヤーの座標を上書きして強制移動させる！
                    player->SetTranslate(pPos);
                }
            }
        } else {
            isSuctionActive_ = false;
        }

        // ==========================================
        // ★ 攻撃3：大爆発（足元範囲ドカン！）
        // ==========================================
        // 吸引が終わった瞬間（330）に爆発スタート！
        if (attackTimer_ == 330) {
            isExplosionActive_ = true;
            explosionScale_ = { 0.1f, 0.1f, 0.1f };
        }

        if (isExplosionActive_) {
            // 猛スピードで巨大化していく！
            explosionScale_.x += 0.5f;
            explosionScale_.z += 0.5f;
            explosionScale_.y = 1.0f; // 平面なので高さは固定

            // 最大サイズ（15.0f）まで広がったら終了
            if (explosionScale_.x > 7.5f) {
                isExplosionActive_ = false;
                // ★ タイマーを -100 にリセットし、再び「ミサイル → ジャンプ」のループへ戻る！
                attackTimer_ = -100;
            }

            explosion_->SetTranslate({ bossPos_.x, 0.01f, bossPos_.z }); // ボスの足元
            explosion_->SetScale(explosionScale_);
            explosion_->Update();
        }
    }

    // ==========================================
    // 全形態共通：腕の移動処理
    // ==========================================
    Vector3 leftShoulder = { bossPos_.x - 0.5f, bossPos_.y, bossPos_.z };
    switch (leftPunchState_) {
    case PunchState::kIdle: leftArmPos_ = leftShoulder; break;
    case PunchState::kPunch:
        leftArmPos_.x += leftArmVelocity_.x; leftArmPos_.y += leftArmVelocity_.y; leftArmPos_.z += leftArmVelocity_.z;
        leftPunchTimer_++;
        if (leftPunchTimer_ >= 100) { leftPunchState_ = PunchState::kReturn; leftPunchTimer_ = 0; }
        break;
    case PunchState::kReturn:
        float dx = leftShoulder.x - leftArmPos_.x; float dy = leftShoulder.y - leftArmPos_.y; float dz = leftShoulder.z - leftArmPos_.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (dist > 0.5f) { leftArmPos_.x += (dx / dist) * 0.5f; leftArmPos_.y += (dy / dist) * 0.5f; leftArmPos_.z += (dz / dist) * 0.5f; } else { leftArmPos_ = leftShoulder; leftPunchState_ = PunchState::kIdle; }
        break;
    }

    Vector3 rightShoulder = { bossPos_.x + 0.5f, bossPos_.y, bossPos_.z };
    switch (rightPunchState_) {
    case PunchState::kIdle: rightArmPos_ = rightShoulder; break;
    case PunchState::kPunch:
        rightArmPos_.x += rightArmVelocity_.x; rightArmPos_.y += rightArmVelocity_.y; rightArmPos_.z += rightArmVelocity_.z;
        rightPunchTimer_++;
        if (rightPunchTimer_ >= 100) { rightPunchState_ = PunchState::kReturn; rightPunchTimer_ = 0; }
        break;
    case PunchState::kReturn:
        float dx = rightShoulder.x - rightArmPos_.x; float dy = rightShoulder.y - rightArmPos_.y; float dz = rightShoulder.z - rightArmPos_.z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (dist > 0.5f) { rightArmPos_.x += (dx / dist) * 0.5f; rightArmPos_.y += (dy / dist) * 0.5f; rightArmPos_.z += (dz / dist) * 0.5f; } else { rightArmPos_ = rightShoulder; rightPunchState_ = PunchState::kIdle; }
        break;
    }

    // ==========================================
    // 全形態共通：大技（ジャンプ＆衝撃波）の処理
    // ==========================================
    if (isMovingToEdge_) {
        bossPos_.z += (phase_ == 2) ? 0.4f : 0.2f; // 第2形態は下がるのも速い
        if (bossPos_.z >= 15.0f) {
            bossPos_.z = 15.0f;
            isMovingToEdge_ = false;
            isJumping_ = true;
            velocityY_ = (phase_ == 2) ? 0.6f : 0.6f; // 第2形態は初回大ジャンプ
        }
    }
    if (isJumping_) {
        bossPos_.y += velocityY_;
        velocityY_ -= 0.05f; // 重力

        if (bossPos_.y <= 0.0f) {
            bossPos_.y = 0.0f;
            isJumping_ = false;

            if (phase_ == 1) {
                attackTimer_ = 0; // 第1形態はここで攻撃終了
            } else {
                jumpCount_++; // 第2形態はジャンプ回数をカウント
            }

            // 着地したら衝撃波！
            isShockwaveActive_ = true;
            shockwaveScale_ = { 20.0f, 0.1f, 2.0f };
            shockwavePos_ = { bossPos_.x, 0.01f, bossPos_.z };
        }
    }
    if (isShockwaveActive_) {
        shockwavePos_.z -= (phase_ == 2) ? 0.6f : 0.1f; // 第2形態は波も速い！(波の速度)

        if (shockwavePos_.z < -20.0f) {
            isShockwaveActive_ = false;

            if (phase_ == 2 && jumpCount_ < 3) {
                // 第2形態：まだ3回ジャンプしてなければ次を飛ぶ！
                isJumping_ = true;
                velocityY_ = 0.4f; // 2回目以降は少し低いジャンプ
            } else {
                // 終了して定位置に戻る
                isReturningToCenter_ = true;
            }
        }

        shockwave_->SetTranslate(shockwavePos_);
        shockwave_->SetScale(shockwaveScale_);
        shockwave_->SetRotate({ 0.0f, 0.0f, 0.0f });
        shockwave_->Update();
    }
    if (isReturningToCenter_) {
        bossPos_.z -= (phase_ == 2) ? 0.4f : 0.2f; // 戻るのも速い
        if (bossPos_.z <= 10.0f) {
            bossPos_.z = 10.0f;
            isReturningToCenter_ = false;
            attackTimer_ = (phase_ == 2) ? 100 : 0; // 第2形態は少し休む
        }
    }

    // ==========================================
    // ★ 追加：ミサイルの移動とホーミング処理
    // ==========================================
    for (int i = 0; i < kMaxMissiles; i++) {
        if (isMissileActive_[i]) {

            // 寿命が残っている間だけ、プレイヤーの方へ曲がる（ホーミング）
            if (missileHomingTimer_[i] > 0) {
                missileHomingTimer_[i]--; // タイマーを減らす

                if (player) {
                    Vector3 pPos = player->GetTranslate();
                    Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z };

                    float dx = targetPos.x - missilePos_[i].x;
                    float dy = targetPos.y - missilePos_[i].y;
                    float dz = targetPos.z - missilePos_[i].z;
                    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (distance > 0.0f) {
                        float speed = 0.2f; // ミサイルの最高速度
                        Vector3 desiredVel = { (dx / distance) * speed, (dy / distance) * speed, (dz / distance) * speed };

                        // ★ プロの技：「今の速度」と「行きたい方向」を混ぜることで、美しいカーブを描く！
                        missileVelocity_[i].x = (missileVelocity_[i].x * 0.95f) + (desiredVel.x * 0.05f);
                        missileVelocity_[i].y = (missileVelocity_[i].y * 0.95f) + (desiredVel.y * 0.05f);
                        missileVelocity_[i].z = (missileVelocity_[i].z * 0.95f) + (desiredVel.z * 0.05f);
                    }
                }
            }
            // ※ タイマーが0になったら上の if文 を無視するので、速度(Velocity)が更新されず、
            //    そのままの角度で「直進」し続けます！（＝ホーミング切れ）

            // 速度を足して移動させる
            missilePos_[i].x += missileVelocity_[i].x;
            missilePos_[i].y += missileVelocity_[i].y;
            missilePos_[i].z += missileVelocity_[i].z;

            // 2. ★追加：進行方向を向かせるための回転計算
            // atan2(x, z) で、XZ平面上の進んでいる角度を求めます
            float angleY = std::atan2(missileVelocity_[i].x, missileVelocity_[i].z);

            // 上下方向（ピッチ）の角度も計算（y と 平面の速度から算出）
            float velocityXZ = std::sqrt(missileVelocity_[i].x * missileVelocity_[i].x + missileVelocity_[i].z * missileVelocity_[i].z);
            float angleX = std::atan2(-missileVelocity_[i].y, velocityXZ);

            // 地面に当たるか、画面のずっと奥/手前に行ったら消滅
            if (missilePos_[i].y <= 0.0f || missilePos_[i].z < -30.0f || missilePos_[i].z > 30.0f) {
                isMissileActive_[i] = false;
            } else {
                // 3Dオブジェクトに座標をセットして更新
                missiles_[i]->SetTranslate(missilePos_[i]);
                missiles_[i]->SetScale({ 0.5f, 0.5f, 0.5f }); // 弾のサイズ
                missiles_[i]->SetRotate({ angleX, angleY, 0.0f });
                missiles_[i]->Update();
            }
        }
    }

    // ==========================================
    // 最後に座標をセット
    // ==========================================
    float yOffset = 1.0f;
    objectBody_->SetTranslate({ bossPos_.x, bossPos_.y + yOffset, bossPos_.z });
    objectLeftArm_->SetTranslate({ leftArmPos_.x, leftArmPos_.y + yOffset, leftArmPos_.z });
    objectRightArm_->SetTranslate({ rightArmPos_.x, rightArmPos_.y + yOffset, rightArmPos_.z });

    objectBody_->Update();
    objectLeftArm_->Update();
    objectRightArm_->Update();
}

void Boss::Draw() {
    if (objectBody_) { objectBody_->Draw(); }

    // ★ 修正：第1形態（phase_ == 1）の時だけ腕を描画する！
    if (phase_ == 1) {
        if (objectLeftArm_) { objectLeftArm_->Draw(); }
        if (objectRightArm_) { objectRightArm_->Draw(); }
    }

    if (isShockwaveActive_ && shockwave_) {
        shockwave_->Draw();
    }

    for (int i = 0; i < kMaxMissiles; i++) {
        if (isMissileActive_[i] && missiles_[i]) {
            missiles_[i]->Draw();
        }
    }

    if (isExplosionActive_ && explosion_) {
        explosion_->Draw();
    }
}

Boss::~Boss() {
    delete objectBody_;
    delete objectLeftArm_;
    delete objectRightArm_;
    delete shockwave_;

    // ★ 追加：ミサイルのメモリを解放する
    for (int i = 0; i < kMaxMissiles; i++) {
        delete missiles_[i];
    }

    delete explosion_;
}

