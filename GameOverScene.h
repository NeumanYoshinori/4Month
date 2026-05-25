#pragma once
#include "Boss.h" 
#include "BaseScene.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Skydome.h"

class Player;
class ModelCommon;
class Object3dCommon;
class Camera;

class GameOverScene : public BaseScene
{
public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera) override;
    void Update(Player* player) override;
    void Draw() override;
    void Finalize() override;

    ~GameOverScene();

    void SetSpriteCommon(class SpriteCommon* spriteCommon) { (void)spriteCommon; }
    //void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
    /*ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
    
    SpriteCommon* spriteCommon_ = nullptr;
    Sprite* gameOverSprite_ = nullptr;*/

    //3D
    Object3dCommon* object3dCommon_ = nullptr;
    Camera* camera_ = nullptr;

    Object3d* gameOverTextObject_ = nullptr;
    Skydome* skydome_ = nullptr;
    Object3d* EnterTextObject_ = nullptr;

};