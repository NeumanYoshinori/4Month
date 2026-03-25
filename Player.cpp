#include "Player.h"
#include "Object3dCommon.h"
#include "Input.h"

using namespace std;
using namespace MathFunction;

void Player::Initialize(Object3dCommon* object3dCommon) {
	// 引数で受け取ってメンバ変数に記録する
	object3dCommon_ = object3dCommon;

	dxBase_ = object3dCommon_->GetDxBase();

	ModelManager::GetInstance()->LoadModel("cube.obj");

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
	// 1. マウスによる視点・向きの操作（★無限回転対応）
	// ==========================================
	if (camera_) {
		// 現在のマウス座標を取得
		POINT currentMousePos;
		GetCursorPos(&currentMousePos);

		// ★画面の中心座標
		// ※ご自身のゲーム画面の解像度に合わせて変更してください
		// 例：1280x720のウィンドウなら 640 と 360 にします
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

		// プレイヤーが向いている「正面」と「右」のベクトルを計算
		Vector3 forward = { std::sin(transform.rotate.y), 0.0f, std::cos(transform.rotate.y) };
		Vector3 right = { std::cos(transform.rotate.y), 0.0f, -std::sin(transform.rotate.y) };

		// W・Sキーで正面・後ろへ移動
		if (input->PushKey(DIK_W)) {
			transform.translate.x += forward.x * speed;
			transform.translate.z += forward.z * speed;
		}
		if (input->PushKey(DIK_S)) {
			transform.translate.x -= forward.x * speed;
			transform.translate.z -= forward.z * speed;
		}
		// A・Dキーで左・右へカニ歩き（ストレイフ）移動
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

		// スペースキーが押された瞬間 ＆ 地面にいる時だけジャンプ！
		if (input->TriggerKey(DIK_SPACE) && isGrounded) {
			velocityY = jumpSpeed;
			isGrounded = false; // 空中判定にする
		}

		// Y座標に速度（落下・ジャンプ）を足し込む
		transform.translate.y += velocityY;

		// 簡易的な地面との当たり判定 (Y=0.0f を地面とする場合)
		// ※もしレイキャストやAABBの処理を既に作っていた場合は、ここをそちらに差し替えてください
		float groundHeight = 0.0f;

		if (transform.translate.y <= groundHeight) {
			// 地面にめり込んだら、地面の高さに押し戻す
			transform.translate.y = groundHeight;
			// これ以上落ちないので速度をゼロにする
			velocityY = 0.0f;
			// 地面についたフラグをON
			isGrounded = true;
		} else {
			// 地面より上にいるなら空中
			isGrounded = false;
		}
	}

	// ==========================================
	// ★ 追加：弾の発射処理（チャージショット対応）
	// ==========================================
	// 左クリックが押されているかチェック (0x8000 で押下状態を判定)
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState('M') & 0x8000)) {
		isCharging_ = true;
		chargeTimer_++;
	} else {
		// 左クリックを離した瞬間
		if (isCharging_) {
			// 180フレーム（約3秒）以上溜めていたらチャージショット！
			FireBullet(chargeTimer_ >= 180);
			isCharging_ = false;
			chargeTimer_ = 0;
		}
	}

	// 弾の更新（移動と寿命管理）
	for (auto it = bullets_.begin(); it != bullets_.end(); ) {
		Bullet* b = *it;
		b->lifeTimer--;

		// 寿命が尽きたか、ボスに当たって消滅フラグが立ったら削除
		if (b->lifeTimer <= 0 || b->isDead) {
			delete b->object3d;
			delete b;
			it = bullets_.erase(it);
			continue;
		}

		// 弾を前へ飛ばす
		b->position.x += b->velocity.x;
		b->position.y += b->velocity.y;
		b->position.z += b->velocity.z;

		// 3Dモデルに座標を適用
		b->object3d->SetTranslate(b->position);
		b->object3d->Update();

		++it;
	}
	// ==========================================
	
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

		// 地面を Y=0.0f とした場合、カメラの最低高度を 1.0f に制限する
		if (cameraPos.y < 1.0f) {
			cameraPos.y = 1.0f;
		}

		// カメラに位置と角度をセット
		camera_->SetTranslate(cameraPos);
		camera_->SetRotate({ cameraAngleX, transform.rotate.y, 0.0f });
	}

	// ==========================================
	// 4. 行列の計算とデータ転送（既存の処理）
	// ==========================================
// ★ ここに私たちの仕組みを復活させます！
	Vector3 drawPos = transform.translate;
	drawPos.y += 1.0f; // 見た目だけ上に持ち上げる

	// ★ 修正：transform.translate ではなく drawPos を使って描画する！
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, drawPos);
	Matrix4x4 worldViewProjectionMatrix;

	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	} else {
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

	// ★ 追加：弾の描画
	for (Bullet* b : bullets_) {
		b->object3d->Draw();
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

// ==========================================
// ★ 追加：弾を生成して発射する関数
// ==========================================
void Player::FireBullet(bool isCharged) {
	Bullet* b = new Bullet();
	b->object3d = new Object3d();
	b->object3d->Initialize(object3dCommon_);
	b->object3d->SetModel("cube.obj"); // ※とりあえずキューブを弾にします
	b->object3d->SetCamera(camera_);

	// 発射位置（プレイヤーの少し上、胸のあたり）
	b->position = transform.translate;
	b->position.y += 1.0f;

	// 発射方向（カメラの向いている方向＝画面の中央へ飛ばす！）
	float pitch = -cameraAngleX;
	float yaw = transform.rotate.y;
	Vector3 shootDir;
	shootDir.x = std::sin(yaw) * std::cos(pitch);
	shootDir.y = std::sin(pitch);
	shootDir.z = std::cos(yaw) * std::cos(pitch);

	if (isCharged) {
		// チャージショット（デカい・速い・寿命長い）
		float speed = 1.0f;
		b->radius = 3.0f;
		b->lifeTimer = 120;
		b->object3d->SetScale({ 1.5f, 1.5f, 1.5f }); // 見た目もデカく！
		b->velocity = { shootDir.x * speed, shootDir.y * speed, shootDir.z * speed };
	} else {
		// 通常ショット
		float speed = 0.8f;
		b->radius = 0.5f;
		b->lifeTimer = 60;
		b->object3d->SetScale({ 0.2f, 0.2f, 0.2f });
		b->velocity = { shootDir.x * speed, shootDir.y * speed, shootDir.z * speed };
	}

	bullets_.push_back(b);
}