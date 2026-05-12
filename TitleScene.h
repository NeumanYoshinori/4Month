// TitleScene.h
#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "Vector2.h"

class Player;
class Object3dCommon;
class Camera;
class SpriteCommon; 

class TitleScene : public BaseScene {
public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera) override;
    void Update(Player* player) override;
    void Draw() override;
    void Finalize() override;
    ~TitleScene();

  
    void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
    Object3dCommon* object3dCommon_ = nullptr;
    SpriteCommon* spriteCommon_ = nullptr; 
    Sprite* titleSprite_ = nullptr;
};