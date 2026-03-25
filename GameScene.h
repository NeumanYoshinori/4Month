#pragma once
#include "Boss.h" 

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
  
    ~GameScene();

private:
    // ボスのインスタンス
    Boss* boss_ = nullptr;

    Object3d* field_ = nullptr;

   
    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
};