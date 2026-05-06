#pragma once
#include "Boss.h" 
#include "BaseScene.h"

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

private:
	// ボスのインスタンス
	Boss* boss_ = nullptr;

	Object3d* field_ = nullptr;


	ModelCommon* modelCommon_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
};