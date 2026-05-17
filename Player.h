#pragma once
#include <string>
#include <vector>
#include <MathFunction.h>
#include <Transform.h>
#include <wrl.h>
#include <d3d12.h>
#include "DirectXBase.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include <cmath>
#include <algorithm>
#include <list>         
#include "Object3d.h"   


class Object3dCommon;
class Input;

// 3Dオブジェクト
class Player {
public: // メンバ関数
	// 座標変換用行列
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	// 平行光源
	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

	// 初期化
	void Initialize(Object3dCommon* object3dCommon);

	~Player();

	// 更新
	void Update(Input* input);

	// 描画
	void Draw();

	// setter
	void SetModel(Model* model) { model_ = model; }

	// setter
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }

	// getter
	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }

	// setter
	void SetModel(const std::string& filePath);

	// setter
	void SetCamera(Camera* camera) { camera_ = camera; }

	// ==========================================
	// 弾（バレット）の仕組み
	// ==========================================
	struct Bullet {
		Object3d* object3d = nullptr;
		Vector3 position;
		Vector3 velocity;
		float radius;      // 当たり判定の大きさ
		int lifeTimer;     // 寿命（消えるまでの時間）
		bool isDead = false;
	};

	const std::list<Bullet*>& GetBullets() const { return bullets_; }
	void FireBullet(bool isCharged); // 弾を発射する関数

	//void OnDamage(int damage = 1) {
	//	if (isInvincible_) return; // 無敵時間中なら無視

	//	hp_ -= damage;
	//	if (hp_ < 0) hp_ = 0;

	//	// ダメージを受けた後の無敵タイマーなどをセットするとよりゲームらしくなります
	//	isInvincible_ = true;
	//	invincibleTimer_ = 60; // 1秒間無敵など
	//}


	// ==========================================
	// 自機のHP・ダメージ・死亡処理
	// ==========================================
	//int hp_ = 10;                 // 自機のHP
	//bool isDead_ = false;        // 死んでいるかどうか
	int invincibilityTimer_ = 0; // 無敵時間タイマー

	bool IsDead() const { return isDead_; } // 外から死んでいるか確認する用
	void OnDamage();                        // ダメージを受けた時に呼ぶ関数

	bool isCinematic_ = false;
	void SetCinematic(bool isCinematic) { isCinematic_ = isCinematic; }

	// 現在の速度を受け取るための関数
	void SetSpeed(float speed) { currentSpeed_ = speed; }

private:
	// 座標変換行列データ作成
	void CreateTransformationMatrixData();

	// 平行光源データ作成
	void CreateDirectionalLight();

	// Object3DCommonのポインタ
	Object3dCommon* object3dCommon_ = nullptr;

	// DirectXBase
	DirectXBase* dxBase_ = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource = nullptr; // 座標返還行列リソース
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	// バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightData = nullptr;

	// Transform
	Transform transform;

	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

	// モデル
	Model* model_ = nullptr;

	Object3d* object3d_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	float velocityY = 0.0f;   // Y軸方向の速度（落下やジャンプ）
	float gravity = 0.01f;    // 重力の強さ（毎フレーム下に向かって加速する量）
	float jumpSpeed = 0.2f;   // ジャンプの初速（ジャンプ力）
	bool isGrounded = false;  // 地面についているかどうかのフラグ

	float cameraAngleX = 0.2f;   // 上下の角度（ピッチ）
	float cameraYawOffset = 0.0f;   // 左右の角度（ヨー）
	float cameraDistance = 8.0f; // プレイヤーからカメラまでの距離
	POINT preMousePos = { 0, 0 };  // 前回のマウス座標

	std::list<Bullet*> bullets_;
	int chargeTimer_ = 0;       // 左クリックを長押ししている時間
	bool isCharging_ = false;   // チャージ中かどうか

	int hp_ = 10;
	bool isInvincible_ = false;
	int invincibleTimer_ = 0;
	bool isDead_ = false; // プレイヤー自身の死亡フラグ

	struct ChargeParticle {
		Object3d* object3d = nullptr;
		Vector3 startPos;     // 発生した位置
		Vector3 position;     // 現在の位置
		float progress;       // 中心に向かう進行度 (0.0f ~ 1.0f)
		float speed;          // 吸い込まれるスピード
	};

	std::list<ChargeParticle*> chargeParticles_;

	// パーティクルの生成と更新用関数
	void SpawnChargeParticle(const Vector3& center);
	void UpdateChargeParticles();

	bool isChargeCompleted_ = false;

	bool isSliding_ = false;             // スライド中かどうか
	int slideTimer_ = 0;                 // スライドの残りフレーム数
	const int SLIDE_DURATION = 15;       // スライドを持続するフレーム数
	Vector3 slideDirection_ = {0, 0, 0}; // スライドする方向
	float slideSpeed_ = 0.4f;            // スライド中の移動速度
	
	int slideCooldownTimer_ = 0;         // 連続スライドを防ぐクールダウン
	const int SLIDE_COOLDOWN = 30;       // 再度スライドできるまでのフレーム数
	// 現在の移動速度を保存する変数（初期値は通常速度の 0.1f）
	float currentSpeed_ = 0.1f;

	float playerRadius = 0.25f;
	float wallRadius = 36.8f;
	Vector3 wallPos = { 5.0f, 0.0f, 0.0f };

	bool hitWall_ = false;

	Vector3 direction_ = { 0.0f, 0.0f, 0.0f };
};