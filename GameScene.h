#pragma once
#include "Boss.h" 
#include "BaseScene.h"
#include "Skydome.h"
#include "Sprite.h"

// 既存のエンジンのクラス
class Player;
class ModelCommon;
class Object3dCommon;
class Camera;

class GameScene :public BaseScene {
public:
	void Initialize(Object3dCommon* object3dCommon, Camera* camera)override;
	void Update(Player* player)override;
	void Draw()override;
	void Finalize() override;

	~GameScene();

    Boss* GetBoss() const { return boss_; }


    void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }
private:

    Player* player_ = nullptr;

	// ボスのインスタンス
	Boss* boss_ = nullptr;

	Object3d* field_ = nullptr;

    Object3d* wall_ = nullptr;

    Skydome* skydome_ = nullptr;
   
    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;

    Camera* camera_ = nullptr;

    SceneManager* sceneManager_ = nullptr;

    int cameraReturnTimer_ = 0;       // 戻るためのタイマー
    const int kReturnDuration = 60;   // 60フレーム（1秒）かけて戻る
    Vector3 returnStartPos_;          // 戻り始めた瞬間の座標
    Vector3 returnStartRot_;          // 戻り始めた瞬間の角度
    bool wasCinematicLastFrame_ = false; // 前のフレームで演出中だったか

    SpriteCommon* spriteCommon_ = nullptr;
    Sprite* hpBarSprite_ = nullptr;

};