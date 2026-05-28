#pragma once
#include "Boss.h" 
#include "BaseScene.h"
#include "SpriteCommon.h"
#include "Sprite.h"

class Player;
class ModelCommon;
class Object3dCommon;
class Camera;

class GameOverScene : public BaseScene
{
public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera) override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

    ~GameOverScene();

   
    void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
    
    SpriteCommon* spriteCommon_ = nullptr;
    Sprite* gameOverSprite_ = nullptr;
};