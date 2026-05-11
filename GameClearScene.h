#pragma once
#include "Boss.h" 
#include "BaseScene.h"

// 既存のエンジンのクラス
class Player;
class ModelCommon;
class Object3dCommon;
class Camera;

class GameClearScene : public BaseScene
{

public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera)override;
    void Update(Player* player)override;
    void Draw()override;
    void Finalize() override;

    ~GameClearScene();

private:

    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
};