#pragma once
#include "Boss.h" 
#include "BaseScene.h"
#include "SpriteCommon.h" // 追加
#include "Sprite.h"

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

    // 🌟 追加
    void SetSpriteCommon(SpriteCommon* spriteCommon) { spriteCommon_ = spriteCommon; }

private:
    ModelCommon* modelCommon_ = nullptr;
    Object3dCommon* object3dCommon_ = nullptr;
    // 🌟 追加
    SpriteCommon* spriteCommon_ = nullptr;
    Sprite* gameOverSprite_ = nullptr;
};