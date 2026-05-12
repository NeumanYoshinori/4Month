#pragma once
#include "Boss.h" 
#include "BaseScene.h"
#include "SpriteCommon.h" // 追加
#include "Sprite.h"

class Player;
class ModelCommon;
class Object3dCommon;
class Camera;

class GameClearScene : public BaseScene
{
public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera) override;
    void Update(Player* player) override;
    void Draw() override;
    void Finalize() override;

    ~GameClearScene();

   
    void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
   
    SpriteCommon* spriteCommon_ = nullptr;
    Sprite* clearSprite_ = nullptr;
};