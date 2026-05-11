#pragma once
#include "DirectXBase.h"
#include "Input.h"
#include <SrvManager.h>
#include <TextureManager.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <ModelManager.h>
#include <Object3dCommon.h>
#include <Camera.h>
#include "Player.h"
#include "GameScene.h"
#include "ParticleManager.h"
#include <ParticleEmitter.h>
#include "SceneManager.h"
#include "TitleScene.h"
#include "SceneFactory.h"

class Game
{
	public:
	Game();
	~Game();
	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	bool IsEndRequst() const { return endRequst_; }

private:
	DirectXBase* dxBase_ = nullptr;
	WinApp* winApp_ = nullptr;
	Input* input_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	SpriteCommon* spriteCommon_ = nullptr;
	ModelManager* modelManager_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
	Camera* camera_ = nullptr;
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	ParticleEmitter* particleEmitter_ = nullptr;

	bool endRequst_ = false;

	SceneManager* sceneManager_ = nullptr;
};

