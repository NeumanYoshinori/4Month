// TitleScene.h
#pragma once
#include "BaseScene.h"
#include "Sprite.h"
#include "Vector2.h"

class Player;
class Object3dCommon;
class Camera;
class SpriteCommon; // 🌟 追加

class TitleScene : public BaseScene {
public:
    void Initialize(Object3dCommon* object3dCommon, Camera* camera) override;
    void Update(Player* player) override;
    void Draw() override;
    void Finalize() override;
    ~TitleScene();

    // 🌟 外から SpriteCommon をもらうための窓口を追加
    void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
    Object3dCommon* object3dCommon_ = nullptr;
    SpriteCommon* spriteCommon_ = nullptr; // 🌟 ここに保存する
    Sprite* titleSprite_ = nullptr;
};