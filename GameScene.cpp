#include "GameScene.h"
#include "ModelManager.h"
#include "Player.h"
#include <cmath>
#include "SceneManager.h"
#include "Input.h"

// 引数を受け取るように変更
void GameScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    // 受け取ったポインタをメンバ変数に保存
    object3dCommon_ = object3dCommon;

    camera_ = camera;

    // ==========================================
    // フィールドの初期化
    // ==========================================
    ModelManager::GetInstance()->LoadModel("field.obj");

    field_ = new Object3d();
    field_->Initialize(object3dCommon_);
    field_->SetModel("field.obj");

    // チームメイトが設定していた角度と位置をそのままセット
    field_->SetRotate({ 3.14f, 0.0f, 0.0f });
    field_->SetTranslate({ 0.0f, 0.0f, 5.0f });

    // カメラの紐付けも忘れずに！
    field_->SetCamera(camera);
    // ==========================================


    boss_ = new Boss();
    if (boss_) {
        boss_->Initialize(object3dCommon_, camera);
    }

    skydome_ = new Skydome();
    skydome_->Initialize(object3dCommon_, camera_);

    sceneManager_ = SceneManager::GetInstance();

}

void GameScene::Update(Player* player) {

    if (boss_->isAppearing_ && player->IsDead()) {
        player->SetHP(10);
        player->SetIsDead(false);
        player->SetInvincibilityTimer(0);
        OutputDebugStringA("Player Reset for New Game!\n");

        boss_->hp_ = 30;           // HPを満タンに
        boss_->isDead_ = false;    // 死亡フラグを解除
     

    }

    if (player->IsDead()) {
        
        sceneManager_->ChangeScene("GAMEOVER");
        return;
    }

    if (boss_->IsDead()) {
        sceneManager_->ChangeScene("GAMECLEAR");
        return;
    }

    if (Input::GetInstance()->TriggerKey(DIK_1)) {

        SceneManager::GetInstance()->ChangeScene("GAMEOVER");
    }

    if (Input::GetInstance()->TriggerKey(DIK_2)) {

        SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
    }

    skydome_->Update(camera_);

    // ==========================================
    // プレゼン用魔法のキー（本番が終わったら消す！）
    // ==========================================
    if (boss_) {
        // [8]キー：ボスのHPを残り1にする（次の一撃で第2形態へ）
        if (GetAsyncKeyState('8') & 0x8000) {
            boss_->SetHP(1);
            OutputDebugStringA("Cheat: Boss HP set to 1 !!\n");
        }
        // [9]キー：ボスのHPを0にして、強制的に演出～第2形態をスタート
        if (GetAsyncKeyState('9') & 0x8000) {
            boss_->ForcePhase2();
            OutputDebugStringA("Cheat: Forced Phase 2 !!\n");
        }
    }

    if (GetAsyncKeyState('R') & 0x8000) {
        PostQuitMessage(0); // ゲーム（ウィンドウ）を終了させる命令
        PostMessage(GetActiveWindow(), WM_CLOSE, 0, 0);
    }


    if (field_) {
        field_->Update();
    }

    // 3. ボスの更新処理（移動や攻撃ロジック）を呼ぶ
    if (boss_) {
        boss_->Update(player);
    }

    if (player && boss_) {

        // =======================================================
        // 登場中（isAppearing_）でも、やられ中（IsDying）でもない時だけ判定！
        // =======================================================
        if (!boss_->isAppearing_ && !boss_->IsDying() && !boss_->IsTransitioning()) {

            if (boss_->IsShockwaveActive()) {
                Vector3 pPos = player->GetTranslate();
                Vector3 wavePos = boss_->GetShockwavePos();
                Vector3 waveScale = boss_->GetShockwaveScale();

                if (boss_->GetPhase() == 1) {
                    // ==========================================
                    // 第1形態：直線の箱型判定
                    // ==========================================
                    float diffZ = std::abs(pPos.z - wavePos.z);
                    bool isHitZ = (diffZ < waveScale.z);
                    bool isHitY = (pPos.y < waveScale.y);

                    if (isHitZ && isHitY) {
                        OutputDebugStringA("Hit Shockwave (Phase 1)!!!\n");
                        player->OnDamage();
                    }
                } else if (boss_->GetPhase() == 2) {
                    // ==========================================
                    // 第2形態：広がるリング状判定
                    // ==========================================
                    float dx = pPos.x - wavePos.x;
                    float dz = pPos.z - wavePos.z;
                    float dist = std::sqrt(dx * dx + dz * dz);

                    // dist(プレイヤーまでの距離)と波の半径(waveScale.x)の差が 1.5f 以内ならヒット
                    // pPos.y < 2.0f は、プレイヤーがジャンプして避けた場合は当たらないようにする処理
                    if (std::abs(dist - waveScale.x) < 0.8f && pPos.y < 1.0f) {
                        OutputDebugStringA("Hit Shockwave (Phase 2 Ring)!!!\n");
                        player->OnDamage();
                    }
                }
            }

            // ==========================================
            // 当たり判定（ロケットパンチ左腕 vs プレイヤー）
            // ==========================================
            if (boss_->IsLeftPunching()) {
                Vector3 pPos = player->GetTranslate();
                Vector3 pCenter = { pPos.x, pPos.y + 1.0f, pPos.z };
                Vector3 armPos = boss_->GetLeftArmPos();
                float dx = pCenter.x - armPos.x; float dy = pCenter.y - armPos.y; float dz = pCenter.z - armPos.z;
                float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                if (distance < 1.5f) {
                    OutputDebugStringA("Hit Left Punch!!!\n");
                    player->OnDamage();
                }
            }

            // ==========================================
            // 当たり判定（ロケットパンチ右腕 vs プレイヤー）
            // ==========================================
            if (boss_->IsRightPunching()) {
                Vector3 pPos = player->GetTranslate();
                Vector3 pCenter = { pPos.x, pPos.y + 1.0f, pPos.z };
                Vector3 armPos = boss_->GetRightArmPos();
                float dx = pCenter.x - armPos.x; float dy = pCenter.y - armPos.y; float dz = pCenter.z - armPos.z;
                float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                if (distance < 1.5f) {
                    OutputDebugStringA("Hit Right Punch!!!\n");
                    player->OnDamage();
                }
            }

            // ==========================================
            // 当たり判定（ホーミングミサイル vs プレイヤー）
            // ==========================================
            for (int i = 0; i < Boss::kMaxMissiles; i++) {
                if (boss_->IsMissileActive(i)) {
                    Vector3 pPos = player->GetTranslate();
                    Vector3 pCenter = { pPos.x, pPos.y + 1.0f, pPos.z };
                    Vector3 mPos = boss_->GetMissilePos(i);

                    float dx = pCenter.x - mPos.x; float dy = pCenter.y - mPos.y; float dz = pCenter.z - mPos.z;
                    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (distance < 1.4f) {
                        OutputDebugStringA("Hit Missile!!!\n");
                        player->OnDamage();
                        boss_->DeactivateMissile(i);
                    }
                }
            }

            // ==========================================
            // 当たり判定（爆発範囲攻撃 vs プレイヤー）
            // ==========================================
            if (boss_->IsExplosionActive()) {
                Vector3 pPos = player->GetTranslate();
                Vector3 bPos = boss_->GetPos();
                Vector3 expScale = boss_->GetExplosionScale();

                float dx = pPos.x - bPos.x; float dz = pPos.z - bPos.z;
                float distance = std::sqrt(dx * dx + dz * dz);

                if (distance < expScale.x) {
                    OutputDebugStringA("Hit Explosion!!! (AoE)\n");
                    player->OnDamage();
                }
            }

            // ==========================================
            // 当たり判定（プレイヤーの弾 vs ボス）
            // ==========================================
            const std::list<Player::Bullet*>& bullets = player->GetBullets();
            for (Player::Bullet* b : bullets) {
                if (b->isDead) { continue; }

                Vector3 bPos = b->position;
                Vector3 bossPos = boss_->GetPos();
                Vector3 bossCenter = { bossPos.x, bossPos.y + 1.0f, bossPos.z };

                float dx = bPos.x - bossCenter.x; float dy = bPos.y - bossCenter.y; float dz = bPos.z - bossCenter.z;
                float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                if (distance < 10.0f) {
                    OutputDebugStringA("Hit Boss!!! (Player Attack)\n");
                    b->isDead = true;
                    boss_->OnDamage();
                }
            }

            // ==========================================
            // 当たり判定（プレイヤーの弾 vs ボスの腕 ＝ 反射！）
            // ==========================================
            for (Player::Bullet* b : bullets) {
                if (b->isDead) { continue; }

                if (b->radius >= 3.0f) {
                    if (boss_->IsLeftPunching()) {
                        Vector3 armPos = boss_->GetLeftArmPos();
                        float dx = b->position.x - armPos.x; float dy = b->position.y - armPos.y; float dz = b->position.z - armPos.z;
                        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
                        if (dist < 3.0f) {
                            b->isDead = true;
                            boss_->ReflectLeftPunch();
                        }
                    }
                    if (boss_->IsRightPunching()) {
                        Vector3 armPos = boss_->GetRightArmPos();
                        float dx = b->position.x - armPos.x; float dy = b->position.y - armPos.y; float dz = b->position.z - armPos.z;
                        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
                        if (dist < 3.0f) {
                            b->isDead = true;
                            boss_->ReflectRightPunch();
                        }
                    }
                }
            }

   // ==========================================
   // 鈍足化フィールドの判定
   // ==========================================
            float playerSpeed = 0.1f; // 通常速度
            for (int i = 0; i < Boss::kMaxSpheres; i++) {
                if (boss_->IsSphereActive(i)) {
                    Vector3 pPos = player->GetTranslate();
                    Vector3 sPos = boss_->GetSpherePos(i);
                    float dx = pPos.x - sPos.x;
                    float dz = pPos.z - sPos.z;
                    float dist = std::sqrt(dx * dx + dz * dz);

                    if (dist < boss_->GetSphereRadius()) {
                        playerSpeed = 0.03f; // スフィア内では大幅にスピードダウン
                        break;
                    }
                }
            }
       
            player->SetSpeed(playerSpeed);

        } 

    } 


    // ==========================================
    // 映画的カメラ演出ディレクター
    // ==========================================
    if (camera_ && boss_ && player) {

        // 今現在、何らかの演出（登場・変身・死亡）が再生されているか
        bool isCurrentlyCinematic = (boss_->isAppearing_ || boss_->IsTransitioning() || boss_->IsDying());

        // ---------------------------------------------------
        // A. 演出再生中（登場 / 形態変化 / 撃破）
        // ---------------------------------------------------
        if (isCurrentlyCinematic) {
            player->SetCinematic(true); // プレイヤーのカメラ操作を無効化
            wasCinematicLastFrame_ = true;

            Vector3 bPos = boss_->GetPos();
            Vector3 camPos = { 0,0,0 };
            Vector3 camRot = { 0,0,0 };

            // ① 登場演出（上空から落下して着地）
            if (boss_->isAppearing_) {
                if (bPos.y > 80.0f) {
                    camPos = { bPos.x + 15.0f, 175.0f, bPos.z + 15.0f }; // 上空固定
                    camRot = { 0.8f, -2.35f, 0.0f };
                } else {
                    camPos = { bPos.x, 2.0f, bPos.z - 20.0f }; // 地面で見上げ
                    camRot = { -0.1f, 0.0f, 0.0f };
                    // 着地直後の画面揺れ
                    if (boss_->GetAppearanceTimer() > 0 && boss_->GetAppearanceTimer() < 30) {
                        camPos.x += ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
                        camPos.y += ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
                    }
                }
            }
            // ② 形態変化演出（第1→第2形態への覚醒）
            else if (boss_->IsTransitioning()) {
                int timer = boss_->GetTransitionTimer();
                if (timer < 60) {
                    // 前半1秒：引きの絵（ボスだけがプルプル震える）
                    camPos = { bPos.x, 2.0f, bPos.z - 25.0f };
                    camRot = { -0.05f, 0.0f, 0.0f };
                } else {
                    // 後半2秒：顔にズームして前後ガクガク揺れ
                    camPos = { bPos.x, 2.0f, bPos.z - 15.0f };
                    camRot = { -0.05f, 0.0f, 0.0f };
                    float shakeZ = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
                    camPos.z += shakeZ;
                }
            }
            // ③ 撃破演出（パッパッパッ！とカット切り替え）
            else if (boss_->IsDying()) {
                int timer = boss_->GetDeathTimer();
                if (timer < 60) {
                    camPos = { bPos.x + 4.0f, bPos.y + 3.0f, bPos.z + 4.0f }; // 右後ろ
                    camRot = { 0.3f, -2.4f, 0.0f };
                } else if (timer < 120) {
                    camPos = { bPos.x - 1.0f, bPos.y + 1.5f, bPos.z - 5.0f }; // 左前ドアップ
                    camRot = { 0.0f, 0.3f, -0.3f };
                } else if (timer < 180) {
                    camPos = { bPos.x - 3.0f, bPos.y + 1.0f, bPos.z + 6.0f }; // 左後ろ
                    camRot = { 0.0f, 2.7f, 0.0f };
                } else {
                    camPos = { bPos.x, 2.0f, bPos.z - 15.0f }; // 正面で崩落を見守る
                    camRot = { 0.1f, 0.0f, 0.0f };
                    if (timer < 240) { // 崩れている間の揺れ
                        camPos.x += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
                        camPos.y += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
                    }
                }
            }

            camera_->SetTranslate(camPos);
            camera_->SetRotate(camRot);

            // 「帰還演出」の開始地点として、今のカメラ状態を常に覚えさせておく
            returnStartPos_ = camera_->GetTranslate();
            returnStartRot_ = camera_->GetRotate();
            cameraReturnTimer_ = 0;
        }
        // ---------------------------------------------------
        // B. 演出終了直後：自機カメラへの「帰還」フェーズ
        // ---------------------------------------------------
        else if (wasCinematicLastFrame_ || cameraReturnTimer_ > 0) {

            // 演出が終わった瞬間、タイマーを1秒分（60）セット
            if (wasCinematicLastFrame_) {
                cameraReturnTimer_ = kReturnDuration;
                wasCinematicLastFrame_ = false;
            }

            // 進捗率 t (0.0 ～ 1.0) を計算
            float t = 1.0f - (float)cameraReturnTimer_ / kReturnDuration;

            // 【イージング】SmoothStepで動きを滑らかにする
            t = t * t * (3.0f - 2.0f * t);

            // 目的地（自機カメラの本来の位置）を計算
            Vector3 pPos = player->GetTranslate();
            Vector3 pRot = player->GetRotate();
            float dist = 15.0f;
            float angleX = 0.2f; // Player.cppの設定に合わせる

            Vector3 targetPos;
            targetPos.x = pPos.x - std::sin(pRot.y) * std::cos(angleX) * dist;
            targetPos.y = (pPos.y + 1.5f) + std::sin(angleX) * dist;
            targetPos.z = pPos.z - std::cos(pRot.y) * std::cos(angleX) * dist;
            Vector3 targetRot = { angleX, pRot.y, 0.0f };

            // Lerp（補間）で中間地点の座標と角度を出す
            Vector3 currentPos = {
                returnStartPos_.x + (targetPos.x - returnStartPos_.x) * t,
                returnStartPos_.y + (targetPos.y - returnStartPos_.y) * t,
                returnStartPos_.z + (targetPos.z - returnStartPos_.z) * t
            };
            Vector3 currentRot = {
                returnStartRot_.x + (targetRot.x - returnStartRot_.x) * t,
                returnStartRot_.y + (targetRot.y - returnStartRot_.y) * t,
                returnStartRot_.z + (targetRot.z - returnStartRot_.z) * t
            };

            camera_->SetTranslate(currentPos);
            camera_->SetRotate(currentRot);

            cameraReturnTimer_--;
            player->SetCinematic(true); // 戻りきるまでは勝手に動かされないように固定
        }
        // ---------------------------------------------------
        // C. 通常プレイ（自由操作）
        // ---------------------------------------------------
        else {
            player->SetCinematic(false);
        }
    }

} // <- GameScene::Update 関数の終わりのカッコ

void GameScene::Draw() {

    if (skydome_) {
        skydome_->Draw(); 
    }

    if (field_) {
        field_->Draw();
    }

    // 4. ボスの描画処理を呼ぶ
    if (boss_) {
        boss_->Draw();
    }
}

void GameScene::Finalize()
{
}

// 忘れがちな後片付け
GameScene::~GameScene() {
    delete boss_;
    boss_ = nullptr;

    delete field_;
    field_ = nullptr;

    delete skydome_;
    skydome_ = nullptr;
}