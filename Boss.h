#pragma once

#include "Object3d.h"
// ※ #include "Model.h" はもう要りません！

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
    // ★ 追加：GameSceneにロケットパンチの情報を教える窓口
    // ==========================================
    // パンチ中（待機状態じゃない）かどうか
    bool IsLeftPunching() const { return leftPunchState_ != PunchState::kIdle; }
    bool IsRightPunching() const { return rightPunchState_ != PunchState::kIdle; }

    // 腕の現在のワールド座標（前回見た目を +1.0f 持ち上げたので、ここでも合わせます）
    Vector3 GetLeftArmPos() const { return leftArmPos_; }
    Vector3 GetRightArmPos() const { return rightArmPos_; }

private:
    // Model* modelBody_ などは全部消してOKです！

    // ==========================================
    // 配置・描画に関する変数（3Dオブジェクト）のみ残す
    // ==========================================
    Object3d* objectBody_ = nullptr;
    Object3d* objectLeftArm_ = nullptr;
    Object3d* objectRightArm_ = nullptr;


    // ==========================================
    // ★ボスのパラメータ
    // ==========================================
    // ボスの基本座標（常にここを基準にする）
    Vector3 bossPos_ = { 0.0f, 0.0f, 10.0f };

    // ロケットパンチの状態を表す「列挙型（enum）」
    enum class PunchState {
        kIdle,   // 待機
        kPunch,  // 飛んでる
        kReturn  // 戻ってる
    };

    // 右腕の現在の状態
    PunchState rightPunchState_ = PunchState::kIdle;

    // ★ 追加：右腕の現在のワールド座標と、寿命タイマー
    Vector3 rightArmPos_ = { 0.0f, 0.0f, 0.0f };
    int rightPunchTimer_ = 0;

    //左腕の場合
    PunchState leftPunchState_ = PunchState::kIdle;
    // ★ 追加：左腕の現在のワールド座標と、寿命タイマー
    Vector3 leftArmPos_ = { 0.0f, 0.0f, 0.0f };
    int leftPunchTimer_ = 0;

    // 自動でパンチを撃たせるためのタイマー
    int attackTimer_ = 0;


    // ==========================================
    // ★ 追加：ジャンプ攻撃用のパラメータ
    // ==========================================   
    bool isMovingToEdge_ = false;
    bool isJumping_ = false;  // ジャンプ中かどうか
    float velocityY_ = 0.0f;  // Y軸の速度（プラスなら上昇、マイナスなら落下）
    bool isReturningToCenter_ = false;
    // ==========================================
    // ★ 追加：衝撃波用のパラメータ
    // ==========================================
    Object3d* shockwave_ = nullptr;
    bool isShockwaveActive_ = false; // 衝撃波が出ているか
    Vector3 shockwaveScale_ = { 0.1f, 0.1f, 0.1f }; // 衝撃波の大きさ
    Vector3 shockwavePos_ = { 0.0f, 0.0f, 0.0f };   // 衝撃波の位置

    // 今後HPなどを追加
};