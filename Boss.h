#pragma once

#include "Object3d.h"


class Object3dCommon;
class Player;

class Boss {
public:
    // 引数を Object3dCommon だけに変更
    void Initialize(Object3dCommon* object3dCommon, Camera* camera);
    void Update(Player* player);
    void Draw();

    ~Boss();

    bool IsShockwaveActive() const { return isShockwaveActive_; }
    Vector3 GetShockwavePos() const { return shockwavePos_; }
    Vector3 GetShockwaveScale() const { return shockwaveScale_; }

    // ==========================================
    // GameSceneにロケットパンチの情報を教える窓口
    // ==========================================
    // パンチ中（待機状態じゃない）かどうか
    bool IsLeftPunching() const { return leftPunchState_ != PunchState::kIdle; }
    bool IsRightPunching() const { return rightPunchState_ != PunchState::kIdle; }

  
    Vector3 GetLeftArmPos() const { return leftArmPos_; }
    Vector3 GetRightArmPos() const { return rightArmPos_; }


    // ==========================================
    // プレゼン＆進行用のHP・フェーズ管理
    // ==========================================
    int hp_ = 10;
    Vector3 GetPos() const { return bossPos_; }

    // ==========================================
     // ボスの死亡フラグ
     // ==========================================
    bool isDead_ = false;
    bool IsDead() const { return isDead_; }

    // ダメージを受けた時の処理
    void OnDamage() {
        // 死んでいない、かつ演出中でない時だけダメージを受ける
        if (hp_ > 0 && !isTransitioning_ && !isDead_) {
            hp_ -= 1;

            // HPが0になったら！
            if (hp_ <= 0) {
                if (phase_ == 1) {
                    isTransitioning_ = true; // 第1形態なら形態変化
                } else if (phase_ == 2) {
                    isDead_ = true;          // ★ 第2形態なら死亡
                    OutputDebugStringA("BOSS DEFEATED!!!\n"); // コンソールに撃破メッセージ
                }
            }
        }
    }


    void ReflectLeftPunch() { leftPunchState_ = PunchState::kReflected; }
    void ReflectRightPunch() { rightPunchState_ = PunchState::kReflected; }

    // スタンタイマー（0より大きい時は気絶中）
    int stunTimer_ = 0;


    // チート関数
    void SetHP(int hp) { hp_ = hp; }
    void ForcePhase2() {
        if (phase_ == 1) {
            hp_ = 0;
            isTransitioning_ = true;
        }
    }


    // ==========================================
    // 第2形態ホーミングミサイル用
    // ==========================================
    static const int kMaxMissiles = 4; // 左右の肩から1発ずつ（計2発）
    bool IsMissileActive(int index) const { return isMissileActive_[index]; }
    Vector3 GetMissilePos(int index) const { return missilePos_[index]; }

    // ==========================================
    // 第2形態 吸引＆爆発攻撃用
    // ==========================================
    bool IsExplosionActive() const { return isExplosionActive_; }
    Vector3 GetExplosionScale() const { return explosionScale_; }

private:
    // Model* modelBody_ などは全部消してOKです！

    // ==========================================
    // 配置・描画に関する変数（3Dオブジェクト）のみ残す
    // ==========================================
    Object3d* objectBody_ = nullptr;
    Object3d* objectLeftArm_ = nullptr;
    Object3d* objectRightArm_ = nullptr;


    // ==========================================
    // ボスのパラメータ
    // ==========================================
    // ボスの基本座標（常にここを基準にする）
    Vector3 bossPos_ = { 0.0f, 0.0f, 10.0f };

    // ロケットパンチの状態を表す「列挙型（enum）」
    enum class PunchState {
        kIdle,   // 待機
        kPunch,  // 飛んでる
        kReturn,  // 戻ってる
        kReflected // プレイヤーに弾き返された状態！
    };

    // 右腕の現在の状態
    PunchState rightPunchState_ = PunchState::kIdle;

    // ★ 追加：右腕の現在のワールド座標と、寿命タイマー
    Vector3 rightArmPos_ = { 0.0f, 0.0f, 0.0f };
    int rightPunchTimer_ = 0;
    Vector3 rightArmVelocity_ = { 0.0f, 0.0f, 0.0f };

    //左腕の場合
    PunchState leftPunchState_ = PunchState::kIdle;
    // ★ 追加：左腕の現在のワールド座標と、寿命タイマー
    Vector3 leftArmPos_ = { 0.0f, 0.0f, 0.0f };
    int leftPunchTimer_ = 0;
    Vector3 leftArmVelocity_ = { 0.0f, 0.0f, 0.0f };

    // 自動でパンチを撃たせるためのタイマー
    int attackTimer_ = 0;


    // ==========================================
    // ジャンプ攻撃用のパラメータ
    // ==========================================   
    bool isMovingToEdge_ = false;
    bool isJumping_ = false;  // ジャンプ中かどうか
    float velocityY_ = 0.0f;  // Y軸の速度（プラスなら上昇、マイナスなら落下）
    bool isReturningToCenter_ = false;

    // 第2形態の連続ジャンプ用
    int jumpCount_ = 0; // 今何回ジャンプしたか
    // ==========================================
    // 衝撃波用のパラメータ
    // ==========================================
    Object3d* shockwave_ = nullptr;
    bool isShockwaveActive_ = false; // 衝撃波が出ているか
    Vector3 shockwaveScale_ = { 0.1f, 0.1f, 0.1f }; // 衝撃波の大きさ
    Vector3 shockwavePos_ = { 0.0f, 0.0f, 0.0f };   // 衝撃波の位置



    // ==========================================
    // フェーズ（形態）管理用パラメータ
    // ==========================================
    int phase_ = 1;                // 現在の形態（1 or 2）
    bool isTransitioning_ = false; // 形態変化の演出中か
    int transitionTimer_ = 0;      // 演出タイマー


    // ==========================================
    // 第2形態ホーミングミサイル用
    // ==========================================
    Object3d* missiles_[kMaxMissiles] = { nullptr, nullptr };
    bool isMissileActive_[kMaxMissiles] = { false, false };
    Vector3 missilePos_[kMaxMissiles];
    Vector3 missileVelocity_[kMaxMissiles];
    int missileHomingTimer_[kMaxMissiles] = { 0, 0 }; // ホーミングの寿命タイマー
    

    // ==========================================
    // 第2形態 吸引＆爆発攻撃用
    // ==========================================
    bool isSuctionActive_ = false;                  // 吸引中かどうか
    Object3d* explosion_ = nullptr;                 // 爆発の3Dモデル
    bool isExplosionActive_ = false;                // 爆発中かどうか
    Vector3 explosionScale_ = { 0.1f, 0.1f, 0.1f }; // 爆発の広がり具合
};