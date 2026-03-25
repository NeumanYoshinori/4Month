#include "Player.h"
#include "Object3dCommon.h"
#include "Input.h"

using namespace std;
using namespace MathFunction;

void Player::Initialize(Object3dCommon* object3dCommon) {
	// 引数で受け取ってメンバ変数に記録する
	object3dCommon_ = object3dCommon;

	dxBase_ = object3dCommon_->GetDxBase();

	// 座標変換行列データ作成
	CreateTransformationMatrixData();

	// 平行光源データ作成
	CreateDirectionalLight();

	// Transform変数を作る
	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	// デフォルトカメラをセットする
	camera_ = object3dCommon_->GetDefaultCamera();

	GetCursorPos(&preMousePos);
}

void Player::Update(Input* input) {

	// ==========================================
	// 1. マウスによる視点移動
	// ==========================================
	if (camera_) {
		// 現在のマウス座標を取得
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);

		// 画面の中心座標
		int centerX = 1280 / 2;
		int centerY = 720 / 2;

		// 画面の中心から「どれだけマウスを動かしたか」を計算
		float deltaX = static_cast<float>(currentMousePos.x - centerX);
		float deltaY = static_cast<float>(currentMousePos.y - centerY);

		// マウスカーソルを画面の中心に強制的に戻す（無限回転のため）
		SetCursorPos(centerX, centerY);

		float sensitivity = 0.003f; // マウス感度

		// 左右のマウス移動でプレイヤー自身（モデル）を回転させる
		transform.rotate.y += deltaX * sensitivity;

		// 上下のマウス移動でカメラのピッチ（上下角度）を変更する
		cameraAngleX += deltaY * sensitivity;

		// 【上下の制限】（カメラが真上・真下を通り過ぎて裏返らないようにする）
		float maxPitch = 1.2f;  // 見下ろし限界
		float minPitch = -1.0f; // 見上げ限界
		if (cameraAngleX > maxPitch) { cameraAngleX = maxPitch; }
		if (cameraAngleX < minPitch) { cameraAngleX = minPitch; }
	}

	// ==========================================
	// 2. プレイヤーの移動（WASDによるストレイフ移動）
	// ==========================================
	if (input) {
		float speed = 0.1f;

		// プレイヤーが向いている向きベクトルを計算
		Vector3 forward = { std::sin(transform.rotate.y), 0.0f, std::cos(transform.rotate.y) };
		Vector3 right = { std::cos(transform.rotate.y), 0.0f, -std::sin(transform.rotate.y) };

		// WASD移動
		if (input->PushKey(DIK_W)) {
			transform.translate.x += forward.x * speed;
			transform.translate.z += forward.z * speed;
		}
		if (input->PushKey(DIK_S)) {
			transform.translate.x -= forward.x * speed;
			transform.translate.z -= forward.z * speed;
		}
		if (input->PushKey(DIK_A)) {
			transform.translate.x -= right.x * speed;
			transform.translate.z -= right.z * speed;
		}
		if (input->PushKey(DIK_D)) {
			transform.translate.x += right.x * speed;
			transform.translate.z += right.z * speed;
		}

		// 重力を加算して落下させる
		velocityY -= gravity;

		// スペースキーが押された瞬間 ＆ 地面にいる時だけジャンプ
		if (input->TriggerKey(DIK_SPACE) && isGrounded) {
			velocityY = jumpSpeed;
			isGrounded = false; // 空中判定にする
		}

		// Y座標に速度（落下・ジャンプ）を足し込む
		transform.translate.y += velocityY;

		// 簡易的な地面との当たり判定 (Y=0.0f を地面とする場合)
		float groundHeight = 0.0f;// 後で変える

		if (transform.translate.y <= groundHeight) {
			// 地面にめり込んだら、地面の高さに押し戻す
			transform.translate.y = groundHeight;
			// これ以上落ちないので速度をゼロにする
			velocityY = 0.0f;
			// 地面についたフラグをON
			isGrounded = true;
		}
		else {
			// 地面より上にいるなら空中
			isGrounded = false;
		}
	}
	// ==========================================
	// 3. カメラの配置（プレイヤーを中央に捉える）
	// ==========================================
	if (camera_) {
		float cameraDistance = 15.0f; // プレイヤーからカメラまでの距離

		// 注視点をプレイヤーの中央（頭の高さなど）に設定する
		Vector3 targetPos = transform.translate;
		targetPos.y += 1.5f;

		// 球座標系の計算を使って、プレイヤーの後ろにカメラを配置する
		Vector3 cameraPos;
		cameraPos.x = targetPos.x - std::sin(transform.rotate.y) * std::cos(cameraAngleX) * cameraDistance;
		cameraPos.y = targetPos.y + std::sin(cameraAngleX) * cameraDistance;
		cameraPos.z = targetPos.z - std::cos(transform.rotate.y) * std::cos(cameraAngleX) * cameraDistance;

		// カメラに位置と角度をセット
		camera_->SetTranslate(cameraPos);
		camera_->SetRotate({ cameraAngleX, transform.rotate.y, 0.0f });
	}

	// ==========================================
	// 射撃と溜め攻撃の処理
	// ==========================================
	bool currentLButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	if (currentLButton) {
		if (!prevLButton) {
			// 💥 基本攻撃！（速さ 2.0f, 大きさ 0.2f, 寿命 60フレーム）
			FireBullet(2.0f, 0.2f, 60);
		}
		chargeTimer++;
	}
	else {
		if (prevLButton) {
			if (chargeTimer >= 180) {
				// 溜め攻撃！（速さ 3.0f, 大きさ 1.5f, 寿命 120フレーム）
				FireBullet(3.0f, 1.5f, 120);
			}
			chargeTimer = 0;
		}
	}
	prevLButton = currentLButton;

	// ==========================================
	//　弾の移動
	// ==========================================
	for (auto it = bullets_.begin(); it != bullets_.end(); ) {
		Bullet* b = *it;

		// 座標を速度分だけ移動させる
		b->position.x += b->velocity.x;
		b->position.y += b->velocity.y;
		b->position.z += b->velocity.z;

		// Object3dに座標をセットして更新
		b->obj->SetTranslate(b->position);
		b->obj->Update();

		// 寿命を減らす
		b->lifeTimer--;
		if (b->lifeTimer <= 0) {
			b->isDead = true;
		}

		// もし死んでいたらリストから消し、メモリを解放する
		if (b->isDead) {
			delete b->obj;
			delete b;
			it = bullets_.erase(it);
		}
		else {
			++it;
		}
	}

	// ==========================================
	// 4. 行列の計算とデータ転送（既存の処理）
	// ==========================================
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix;

	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	}
	else {
		worldViewProjectionMatrix = worldMatrix;
	}

	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
}

void Player::Draw() {

	// コマンドリストを作成
	commandList = dxBase_->GetCommandList();

	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	// 平行光源CBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}

	for (auto bullet : bullets_) {
		bullet->obj->Draw();
	}
}

void Player::SetModel(const std::string& filePath) {
	// モデルを検索
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Player::CreateTransformationMatrixData() {
	// TransformationMatrix用のリソースを作る。
	transformationMatrixResource = dxBase_->CreateBufferResource(sizeof(TransformationMatrix));

	// 書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}

void Player::CreateDirectionalLight() {
	// 平行光源リソースを作る
	directionalLightResource = dxBase_->CreateBufferResource(sizeof(DirectionalLight));

	// 平行光源リソースにデータを書き込むためのアドレスを取得して平行光源構造体のポインタに割り当てる
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;
}

void Player::FireBullet(float speed, float scale, int lifeTime) {

	// 新しい弾を作る
	Bullet* newBullet = new Bullet();

	// 3Dオブジェクトの生成と初期化
	newBullet->obj = new Object3d();
	newBullet->obj->Initialize(object3dCommon_);
	newBullet->obj->SetModel("cube.obj");
	newBullet->obj->SetCamera(camera_);

	// 弾の初期座標（プレイヤーの少し前、少し上）
	newBullet->position = transform.translate;
	newBullet->position.y += 1.0f; // 地面ではなく胸〜銃の高さから発射する

	// プレイヤーが向いている方向(Y軸回転)と、カメラの上下(ピッチ)を使って計算します
	newBullet->velocity.x = std::sin(transform.rotate.y) * speed;
	newBullet->velocity.y = std::sin(-cameraAngleX) * speed; // 見下ろしている時は下へ飛ぶようにマイナスにする
	newBullet->velocity.z = std::cos(transform.rotate.y) * speed;

	// キューブの大きさをセット
	newBullet->obj->SetScale({ scale, scale, scale });

	// 寿命をセット
	newBullet->lifeTimer = lifeTime;

	// リストに弾を追加！
	bullets_.push_back(newBullet);
}
