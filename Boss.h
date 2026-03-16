#pragma once

#include "Object3d.h"
// ※ #include "Model.h" はもう要りません！

class Object3dCommon;

class Boss {
public:
    // 引数を Object3dCommon だけに変更
    void Initialize(Object3dCommon* object3dCommon, Camera* camera);
    void Update();
    void Draw();

    ~Boss();

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

    // 右腕がどれくらい前に飛んでいるかの距離
    float rightArmZ_ = 0.0f;

    //左腕の場合
    PunchState leftPunchState_ = PunchState::kIdle;
    float leftArmZ_ = 0.0f;

    // 自動でパンチを撃たせるためのタイマー
    int attackTimer_ = 0;

    // 今後HPなどを追加
};