#pragma once
#include "Boss.h" 
#include "Skydome.h"

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

    Boss* GetBoss() const { return boss_; }


private:
    // ボスのインスタンス
    Boss* boss_ = nullptr;

    Object3d* field_ = nullptr;

    Skydome* skydome_ = nullptr;
   
    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;

    Camera* camera_ = nullptr;

    int cameraReturnTimer_ = 0;       // 戻るためのタイマー
    const int kReturnDuration = 60;   // 60フレーム（1秒）かけて戻る
    Vector3 returnStartPos_;          // 戻り始めた瞬間の座標
    Vector3 returnStartRot_;          // 戻り始めた瞬間の角度
    bool wasCinematicLastFrame_ = false; // 前のフレームで演出中だったか

};