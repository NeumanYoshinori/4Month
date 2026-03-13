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

private:
    // Model* modelBody_ などは全部消してOKです！

    // ==========================================
    // 配置・描画に関する変数（3Dオブジェクト）のみ残す
    // ==========================================
    Object3d* objectBody_ = nullptr;
    Object3d* objectLeftArm_ = nullptr;
    Object3d* objectRightArm_ = nullptr;

    // 今後HPなどを追加
};