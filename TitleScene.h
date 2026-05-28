// TitleScene.h
#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "Vector2.h"
#include "Object3d.h"
#include "Skydome.h"


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

    void SetSpriteCommon(SpriteCommon* spriteCommon) { (void)spriteCommon; }
   // void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
   /* Object3dCommon* object3dCommon_ = nullptr;
    SpriteCommon* spriteCommon_ = nullptr; 
    Sprite* titleSprite_ = nullptr;*/

    // 3Dモデル用
    Object3d* titleTextObject_ = nullptr;
    Skydome* skydome_ = nullptr;
    Object3d* EnterTextObject_ = nullptr;

    Object3dCommon* object3dCommon_ = nullptr;
    Camera* camera_ = nullptr;

};