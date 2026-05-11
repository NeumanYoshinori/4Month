#pragma once
#include "Object3d.h"
#include "Vector3.h"

class Object3dCommon;
class Camera;

class Skydome {
public:
    // 初期化：モデルの読み込みとセット
    void Initialize(Object3dCommon* object3dCommon, Camera* camera);

    // 更新：カメラの位置に追従させる
    void Update(Camera* camera);

    // 描画
    void Draw();

    ~Skydome();

private:
    Object3d* object_ = nullptr;
    // スカイドームの大きさ（半径）
    // プレイヤーの移動範囲(25.0f)より十分大きく設定します
    const float kScale = 10.0f;
};