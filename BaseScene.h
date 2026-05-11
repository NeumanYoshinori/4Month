#pragma once
class WinApp;
class GraphicsDevice;
class SceneManager;
class Object3dCommon;
class Camera;
class Player;

class BaseScene
{
public:

	virtual ~BaseScene() = default;
	virtual void Initialize(Object3dCommon* object3dCommon, Camera* camera) = 0;
	virtual void Update(Player* player) = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;
};

