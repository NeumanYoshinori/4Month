#pragma once
#include "Boss.h" 
#include "BaseScene.h"
#include "SpriteCommon.h" // 追加
#include "Sprite.h"
#include "Object3d.h"
#include "Skydome.h"


class Player;
class ModelCommon;
class Object3dCommon;
class Camera;

class RuleScene : public BaseScene
{
public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera) override;
    void Update(Player* player) override;
    void Draw() override;
    void Finalize() override;

    ~RuleScene();

    void SetSpriteCommon(class SpriteCommon* spriteCommon) { (void)spriteCommon; }
    // void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
    /* ModelCommon* modelCommon_ = nullptr;
     Object3dCommon* object3dCommon_ = nullptr;

     SpriteCommon* spriteCommon_ = nullptr;
     Sprite* clearSprite_ = nullptr;*/

    Object3dCommon* object3dCommon_ = nullptr;
    Camera* camera_ = nullptr;

    // 3Dモデル用
    Object3d* RuleTextObject_ = nullptr;
    Skydome* skydome_ = nullptr;
    Object3d* EnterTextObject_ = nullptr;

};