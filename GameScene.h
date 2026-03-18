#pragma once
#include "Boss.h" // ボスのヘッダーを忘れずにインクルード

// 既存のエンジンのクラス
class Player;
class ModelCommon;
class Object3dCommon;
class Camera;

class GameScene {
public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera);
    void Update(Player* player);
    void Draw();
    // デストラクタで後片付けをするのが安全です
    ~GameScene();

private:
    // ボスのインスタンス
    Boss* boss_ = nullptr;

    Object3d* field_ = nullptr;

    // エンジン側で用意されている共通部品（すでにあるはずです）
    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
};