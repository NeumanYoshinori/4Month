#include "GameScene.h"
#include "ModelManager.h"
#include "Player.h"
#include <cmath>

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
        // ※ModelCommonは一旦 nullptr で渡しておきます
        boss_->Initialize(object3dCommon_, camera);
    }

}

void GameScene::Update(Player* player) {

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
        // ⬇️ ★ 新規追加：ここに「大きなフタ」をする！
        // 登場中（isAppearing_）でも、やられ中（IsDying）でもない時だけ判定！
        // =======================================================
        if (!boss_->isAppearing_ && !boss_->IsDying()) {

            if (boss_->IsShockwaveActive()) {
                Vector3 pPos = player->GetTranslate();
                Vector3 wavePos = boss_->GetShockwavePos();
                Vector3 waveScale = boss_->GetShockwaveScale();

                float diffZ = std::abs(pPos.z - wavePos.z);
                bool isHitZ = (diffZ < waveScale.z);
                bool isHitY = (pPos.y < waveScale.y);

                if (isHitZ && isHitY) {
                    OutputDebugStringA("Hit Shockwave!!!\n");
                    player->OnDamage();
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

                if (distance < 2.0f) {
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

                if (distance < 2.0f) {
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

        } // ⬅️ ★ 新規追加：ここで当たり判定の「大きな箱」を閉じる！！！

    } // <- if (player && boss_) の閉じカッコ



    // ==========================================mn
    // 🎥 映画的カメラ演出ディレクター（完全版）
    // ==========================================
    if (camera_ && boss_ && player) {

        // ---------------------------------------------------
        // ① 登場演出のカメラ（斜め後ろの上空固定 → 足元着地）
        // ---------------------------------------------------
        if (boss_->isAppearing_) {
            player->SetCinematic(true); // プレイヤーのカメラ操作を奪う

            Vector3 bPos = boss_->GetPos();
            Vector3 camPos;
            Vector3 camRot;

            // --- A. 落下前半：上空（Y=175）の固定カメラから、落ちていくボスを見下ろす ---
            if (bPos.y > 80.0f) {
                // X: +15.0f(右) / Y: 175.0f(上空固定) / Z: +15.0f(背中側)
                camPos = { bPos.x + 15.0f, 175.0f, bPos.z + 15.0f };
                // 少し下を向きつつ、左前（ボスの方向）を向く
                camRot = { 0.8f, -2.35f, 0.0f };
            }
            // --- B. 落下後半 ～ 着地：地面のカメラに切り替えて見上げる ---
            else {
                // 着地点の少し手前で待ち構えるカメラ
                camPos = { bPos.x, 2.0f, bPos.z - 20.0f };
                camRot = { -0.1f, 0.0f, 0.0f }; // 少し上を見上げる角度

                // --- C. 着地した瞬間：画面揺れ（スクリーンシェイク） ---
                // 波エフェクトなしで、着地後最初の30フレーム（0.5秒）だけ激しく揺らす！
                if (boss_->GetAppearanceTimer() > 0 && boss_->GetAppearanceTimer() < 30) {
                    float shakeX = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
                    float shakeY = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
                    camPos.x += shakeX;
                    camPos.y += shakeY;
                }
            }

            camera_->SetTranslate(camPos);
            camera_->SetRotate(camRot);
        }

        // ---------------------------------------------------
        // ② 撃破時のスタイリッシュ・カットイン演出（パッパッパッ！）
        // ---------------------------------------------------
        else if (boss_->IsDying()) {
            player->SetCinematic(true); // プレイヤーのカメラ操作を奪う

            Vector3 bPos = boss_->GetPos();
            Vector3 camPos;
            Vector3 camRot;
            int timer = boss_->GetDeathTimer();

          
            if (timer < 60) {
               
                camPos = { bPos.x + 4.0f, bPos.y + 3.0f, bPos.z + 4.0f };
                camRot = { 0.3f, -2.4f, 0.0f };
            } else if (timer < 120) {
              
                camPos = { bPos.x-1.0f, bPos.y +1.5f, bPos.z -5.0f };
                camRot = { 0.0f, 0.3f, -0.3f };
            } else if (timer < 180) {
              
                camPos = { bPos.x - 3.0f, bPos.y + 1.0f, bPos.z + 6.0f };
                camRot = { 0.0f, 2.7f, 0.0f };
            }
          
            else {
              
                camPos = { bPos.x, 2.0f, bPos.z - 15.0f };
                camRot = { 0.1f, 0.0f, 0.0f };

                // ボスが崩れ落ちている間、画面全体もガタガタ揺らす！
                if (timer < 240) {
                    camPos.x += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
                    camPos.y += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
                }
            }

            camera_->SetTranslate(camPos);
            camera_->SetRotate(camRot);
        }

        // ---------------------------------------------------
        // ③ 普段のゲームプレイ（戦闘中）
        // ---------------------------------------------------
        else {
            // 演出が終わったら、プレイヤーにカメラの操作権をお返しする
            player->SetCinematic(false);
        }
    }

} // <- GameScene::Update 関数の終わりのカッコ

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