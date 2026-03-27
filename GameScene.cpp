#include "GameScene.h"
#include "ModelManager.h"
#include "Player.h"
#include <cmath>

// 引数を受け取るように変更
void GameScene::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
    // 受け取ったポインタをメンバ変数に保存
    object3dCommon_ = object3dCommon;


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
                player->OnDamage();
            }
        }

        // ==========================================
        // 当たり判定（ロケットパンチ左腕 vs プレイヤー）
        // ==========================================
        if (boss_->IsLeftPunching()) {
            Vector3 pPos = player->GetTranslate();
            // プレイヤーの中心点を計算（足元 Y=0.0f から 1.0f 上げたお腹のあたり）
            Vector3 pCenter = { pPos.x, pPos.y + 1.0f, pPos.z };
            Vector3 armPos = boss_->GetLeftArmPos(); // 腕の中心点

            // X, Y, Z のそれぞれの距離の差を求める
            float dx = pCenter.x - armPos.x;
            float dy = pCenter.y - armPos.y;
            float dz = pCenter.z - armPos.z;

            // 3Dの距離を計算（三平方の定理：ルート(x^2 + y^2 + z^2)）
            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            // 当たり判定の大きさ（プレイヤーの半径 + 腕の半径）
            // ※ 腕が大きければ、この数値を 2.0f などに増やします
            float hitRadius = 2.0f;

            if (distance < hitRadius) {
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

            float dx = pCenter.x - armPos.x;
            float dy = pCenter.y - armPos.y;
            float dz = pCenter.z - armPos.z;

            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            float hitRadius = 2.0f;

            if (distance < hitRadius) {
                OutputDebugStringA("Hit Right Punch!!!\n");
                player->OnDamage();
            }
        }

        // ==========================================
        // 当たり判定（ホーミングミサイル vs プレイヤー）
        // ==========================================
        for (int i = 0; i < Boss::kMaxMissiles; i++) {
            // ミサイルが存在している時だけ判定
            if (boss_->IsMissileActive(i)) {
                Vector3 pPos = player->GetTranslate();
                Vector3 pCenter = { pPos.x, pPos.y + 1.0f, pPos.z }; // プレイヤーの中心
                Vector3 mPos = boss_->GetMissilePos(i);              // ミサイルの位置

                float dx = pCenter.x - mPos.x;
                float dy = pCenter.y - mPos.y;
                float dz = pCenter.z - mPos.z;
                float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                float hitRadius = 1.4f; // ミサイルの当たり判定の大きさ

                if (distance < hitRadius) {
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
            Vector3 bPos = boss_->GetPos(); // 爆発の中心（ボスの位置）
            Vector3 expScale = boss_->GetExplosionScale();

            // ZとXの距離（平面での距離）を測る
            float dx = pPos.x - bPos.x;
            float dz = pPos.z - bPos.z;
            float distance = std::sqrt(dx * dx + dz * dz);

            // プレイヤーが爆発のスケール（半径）の内側にいたらヒット！
            if (distance < expScale.x) {
                OutputDebugStringA("Hit Explosion!!! (AoE)\n");
                player->OnDamage();
            }
        }
        // ==========================================
        // 当たり判定（プレイヤーの弾 vs ボス）
        // ==========================================
        // プレイヤーから発射されている全ての弾を取得
        const std::list<Player::Bullet*>& bullets = player->GetBullets();

        for (Player::Bullet* b : bullets) {
            // すでに当たって消える予定の弾は無視
            if (b->isDead) {
                continue;
            }

            Vector3 bPos = b->position;
            Vector3 bossPos = boss_->GetPos();
            Vector3 bossCenter = { bossPos.x, bossPos.y + 1.0f, bossPos.z }; // ボスの胸を狙う

            float dx = bPos.x - bossCenter.x;
            float dy = bPos.y - bossCenter.y;
            float dz = bPos.z - bossCenter.z;
            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            float hitRadius = 10.0f; // ボスの当たり判定

            if (distance < hitRadius) {
                OutputDebugStringA("Hit Boss!!! (Player Attack)\n");

                b->isDead = true;   // 弾を消す！
                boss_->OnDamage();  // ボスのHPを減らす！
            }
        }

        // ==========================================
         // 当たり判定（プレイヤーの弾 vs ボスの腕 ＝ 反射！）
         // ==========================================
        for (Player::Bullet* b : bullets) {
            if (b->isDead) { continue; }

            // チャージ弾（radiusがでかい弾）だけが反射できる仕様にする
            if (b->radius >= 3.0f) {

                // ---------------------------------
                // ① 左腕との判定
                // ---------------------------------
                if (boss_->IsLeftPunching()) {
                    Vector3 armPos = boss_->GetLeftArmPos();
                    float dx = b->position.x - armPos.x;
                    float dy = b->position.y - armPos.y;
                    float dz = b->position.z - armPos.z;
                    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (dist < 3.0f) {
                        OutputDebugStringA("Reflect Left Punch!!!\n");
                        b->isDead = true;
                        boss_->ReflectLeftPunch();
                    }
                }

                // ---------------------------------
                // ② 右腕との判定
                // ---------------------------------
                // ⬇️ IsRightPunching になっていますか？
                if (boss_->IsRightPunching()) {
                    // ⬇️ GetRightArmPos になっていますか？
                    Vector3 armPos = boss_->GetRightArmPos();

                    float dx = b->position.x - armPos.x;
                    float dy = b->position.y - armPos.y;
                    float dz = b->position.z - armPos.z;
                    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (dist < 3.0f) {
                        OutputDebugStringA("Reflect Right Punch!!!\n");
                        b->isDead = true;
                        // ⬇️ ReflectRightPunch になっていますか？            
                        boss_->ReflectRightPunch();
                    }
                }
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