#include "Player.h"
#include "Object3dCommon.h"
#include "Input.h"

using namespace std;
using namespace MathFunction;

void Player::Initialize(Object3dCommon* object3dCommon) {
	// 引数で受け取ってメンバ変数に記録する
	object3dCommon_ = object3dCommon;

	dxBase_ = object3dCommon_->GetDxBase();

	ModelManager::GetInstance()->LoadModel("bullet.obj");

	ModelManager::GetInstance()->LoadModel("player.obj");
	ModelManager::GetInstance()->LoadModel("player_red.obj"); // 赤い自機のモデルを用意してください

	object3d_ = new Object3d();
	object3d_->Initialize(object3dCommon_);

	// 座標変換行列データ作成
	CreateTransformationMatrixData();

	// 平行光源データ作成5r
	CreateDirectionalLight();

	// Transform変数を作る
	transform = { {0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	// デフォルトカメラをセットする
	camera_ = object3dCommon_->GetDefaultCamera();

	int centerX = 1280 / 2;
	int centerY = 720 / 2;
	// ゲーム開始前に、カーソルを強制的に画面の中央にセットしておく
	SetCursorPos(centerX, centerY);
}

void Player::Update(Input* input) {


	// 無敵タイマーを毎フレーム減らす
	if (invincibilityTimer_ > 0) {
		invincibilityTimer_--;
	}

	// もし死んでいたら、この先の操作や更新を一切やらない（＝ゲームオーバーで操作不能になる）
	if (isDead_) {
		return;
	}


	// ==========================================
	// 1. マウスによる視点・向きの操作
	// ==========================================
	if (camera_ && !isCinematic_) {
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
	// 2. プレイヤーの移動
	// ==========================================
	if (input) {
		float speed = currentSpeed_;

		// プレイヤーが向いている「正面」と「右」のベクトルを計算
		Vector3 forward = { std::sin(transform.rotate.y), 0.0f, std::cos(transform.rotate.y) };
		Vector3 right = { std::cos(transform.rotate.y), 0.0f, -std::sin(transform.rotate.y) };

		// --- スライドのクールダウン更新 ---
		if (slideCooldownTimer_ > 0) {
			slideCooldownTimer_--;
		}

		// 【変更】isGrounded の条件を外し、空中でもスライドできるようにしました
		if (!isGrounded && !isSliding_ && slideCooldownTimer_ <= 0 && input->TriggerKey(DIK_SPACE)) {

			// 入力されているキーの方向を計算
			Vector3 inputDir = { 0.0f, 0.0f, 0.0f };
			bool hasDirectionInput = false; // 【追加】方向キーが押されているかのフラグ

			if (input->PushKey(DIK_W)) { inputDir.x += forward.x; inputDir.z += forward.z; hasDirectionInput = true; }
			if (input->PushKey(DIK_S)) { inputDir.x -= forward.x; inputDir.z -= forward.z; hasDirectionInput = true; }
			if (input->PushKey(DIK_A)) { inputDir.x -= right.x; inputDir.z -= right.z; hasDirectionInput = true; }
			if (input->PushKey(DIK_D)) { inputDir.x += right.x; inputDir.z += right.z; hasDirectionInput = true; }

			// 【変更】方向キーが入力されている時だけスライドを発動する
			if (hasDirectionInput) {
				// 斜め入力時に速くなりすぎないようベクトルを正規化（長さを1にする）
				float len = std::sqrt(inputDir.x * inputDir.x + inputDir.z * inputDir.z);
				if (len > 0.0f) {
					inputDir.x /= len;
					inputDir.z /= len;
				}

				// スライド状態に突入
				slideDirection_ = inputDir;
				isSliding_ = true;
				slideTimer_ = SLIDE_DURATION;

				// 空中スライド時に落下速度をリセットして、滞空するようにするなら以下を有効にする
				// velocityY = 0.0f; 
			}
		}

		// --- 実際の移動処理 ---
		if (isSliding_) {
			// スライド中の高速移動（通常の入力は無視される）
			transform.translate.x += slideDirection_.x * slideSpeed_;
			transform.translate.z += slideDirection_.z * slideSpeed_;

			slideTimer_--;
			if (slideTimer_ <= 0) {
				isSliding_ = false; // スライド終了
				slideCooldownTimer_ = SLIDE_COOLDOWN; // クールダウン開始
			}
		}
		else {
			// 通常の移動（スライドしていない時だけWASDで動ける）
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
		}

		// 重力を加算して落下させる
		if (!isSliding_) { velocityY -= gravity; }
		//velocityY -= gravity;

		// スペースキーが押された瞬間 ＆ 地面にいる時だけジャンプ！
		if (input->TriggerKey(DIK_SPACE) && isGrounded && !isSliding_) { // スライド中はジャンプ不可にする
			velocityY = jumpSpeed;
			isGrounded = false; // 空中判定にする
		}

		// Y座標に速度（落下・ジャンプ）を足し込む
		transform.translate.y += velocityY;


		// Y座標に速度（落下・ジャンプ）を足し込む
		transform.translate.y += velocityY;

		// 簡易的な地面との当たり判定
		float groundHeight = 0.0f;

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

	// 1. 無敵時間のカウントダウン
	if (isInvincible_) {
		invincibleTimer_--;
		if (invincibleTimer_ <= 0) {
			isInvincible_ = false;
		}
	}

	// 2. 死亡判定
	if (hp_ <= 0) {
		isDead_ = true;
		// 必要に応じてここで「死亡アニメーション」などを再生
		OutputDebugStringA("PLAYER DEAD\n");
	}

	if (isDead_) return;

	// ==========================================
	// 弾の発射とチャージ処理
	// ==========================================
	// 左クリックを押している間
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState('M') & 0x8000)) {
		isCharging_ = true;
		chargeTimer_++;

		if (chargeTimer_ >= 180) {
			// まだモデルを切り替えていなければ、赤いモデルに変更する
			if (!isChargeCompleted_) {
				isChargeCompleted_ = true;
				object3d_->SetModel("player_red.obj");
			}
		}

		// 20フレーム以上、かつ「チャージ完了前」の時だけパーティクルを出す
		if (chargeTimer_ > 20 && !isChargeCompleted_) {
			// 3フレームに1回パーティクル生成
			if (chargeTimer_ % 3 == 0) {
				Vector3 center = transform.translate;
				center.y += 1.0f;
				SpawnChargeParticle(center);
			}
		}

	}
	else {
		// 左クリックを離した瞬間
		if (isCharging_) {
			FireBullet(isChargeCompleted_);

			// リセット処理
			isCharging_ = false;
			chargeTimer_ = 0;

			if (isChargeCompleted_) {
				isChargeCompleted_ = false;
				object3d_->SetModel("player.obj");
			}
		}
	}

	// ==========================================
	// 弾の更新（移動と寿命管理）
	// ==========================================

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

	UpdateChargeParticles();

	// ==========================================
	// 3. カメラの配置（プレイヤーを中央に捉える）
	// ==========================================
	if (camera_ && !isCinematic_) {
		float cameraDistance = 15.0f;

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
	// 4. 行列の計算とデータ転送
	// ==========================================
	Vector3 drawPos = transform.translate;
	drawPos.y += 1.0f; // 見た目だけ上に持ち上げる

	if (object3d_) {
		object3d_->SetTranslate(drawPos);
		object3d_->SetRotate(transform.rotate);
		object3d_->SetScale(transform.scale);
		object3d_->SetCamera(camera_); // カメラもセット
		object3d_->Update();
	}

	// transform.translate ではなく drawPos を使って描画する
	//Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, drawPos);
	//Matrix4x4 worldViewProjectionMatrix;

	//if (camera_) {
	//	const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
	//	worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	//} else {
	//	worldViewProjectionMatrix = worldMatrix;
	//}

	//transformationMatrixData->WVP = worldViewProjectionMatrix;
	//transformationMatrixData->World = worldMatrix;

	currentSpeed_ = 0.1f;
}

void Player::Draw() {
	//// コマンドリストを作成
	//commandList = dxBase_->GetCommandList();

	//// wvp用のCBufferの場所を設定
	//commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	//// 平行光源CBufferの場所を設定
	//commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	if (isInvincible_) {
		// 4フレームに1回だけ消える演出
		if (invincibleTimer_ % 8 < 4) {
			return;
		}
	}

	// 3Dモデルが割り当てられていれば描画する
	if (object3d_) {
		object3d_->Draw();
	}
	// ★ 修正：死んでいない時だけ自機を描画する！
	if (!isDead_) {
		// 無敵時間中はチカチカ点滅させる（4フレームごとに表示/非表示を切り替え）
		if (invincibilityTimer_ == 0 || invincibilityTimer_ % 4 >= 2) {
			if (object3d_) {
				object3d_->Draw();
			}
		}
	}

	// 弾は、自機が死んでいても画面に残って飛んでいくように別で描画
	for (Bullet* b : bullets_) {
		b->object3d->Draw();
	}

	for (ChargeParticle* p : chargeParticles_) {
		p->object3d->Draw();
	}
}


void Player::SetModel(const std::string& filePath) {
	if (object3d_) {
		object3d_->SetModel(filePath);
	}
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
// 弾を生成して発射する関数
// ==========================================
void Player::FireBullet(bool isCharged) {
	Bullet* newBullet = new Bullet();
	newBullet->object3d = new Object3d();
	newBullet->object3d->Initialize(object3dCommon_);
	newBullet->object3d->SetModel("bullet.obj");
	newBullet->object3d->SetCamera(camera_);

	// 発射位置
	newBullet->position = transform.translate;
	newBullet->position.y += 1.0f;

	// カメラの向いている方向（基本のベクトル）
	float pitch = -cameraAngleX;
	float yaw = transform.rotate.y;
	Vector3 cameraDir;
	cameraDir.x = std::sin(yaw) * std::cos(pitch);
	cameraDir.y = std::sin(pitch);
	cameraDir.z = std::cos(yaw) * std::cos(pitch);

	// ==========================================
	// 画面の奥に向かって撃つ！
	// ==========================================
	// 1. カメラの現在位置を取得
	Vector3 cameraPos = camera_->GetTranslate();

	// 2. 画面のど真ん中、ずっと奥（50先）にある「目標点」を計算する
	float targetDistance = 50.0f;
	Vector3 targetPoint;
	targetPoint.x = cameraPos.x + (cameraDir.x * targetDistance);
	targetPoint.y = cameraPos.y + (cameraDir.y * targetDistance);
	targetPoint.z = cameraPos.z + (cameraDir.z * targetDistance);

	// 3. プレイヤーの胸（newBullet->position）から、目標点へ向かうベクトルを作る
	Vector3 realShootDir;
	realShootDir.x = targetPoint.x - newBullet->position.x;
	realShootDir.y = targetPoint.y - newBullet->position.y;
	realShootDir.z = targetPoint.z - newBullet->position.z;

	// 4. そのベクトルの長さを1に揃える（正規化）
	float length = std::sqrt(realShootDir.x * realShootDir.x + realShootDir.y * realShootDir.y + realShootDir.z * realShootDir.z);
	if (length > 0.0f) {
		realShootDir.x /= length;
		realShootDir.y /= length;
		realShootDir.z /= length;
	}
	// ==========================================

	if (isCharged) {
		// チャージショット（巨大！）
		float speed = 1.0f;
		newBullet->radius = 3.0f;
		newBullet->lifeTimer = 120;
		newBullet->object3d->SetScale({ 1.5f, 1.5f, 1.5f });
		newBullet->velocity = { realShootDir.x * speed, realShootDir.y * speed, realShootDir.z * speed };
	}
	else {
		// 通常ショット
		float speed = 0.8f;
		newBullet->radius = 0.5f;
		newBullet->lifeTimer = 60;
		newBullet->object3d->SetScale({ 0.2f, 0.2f, 0.2f });
		newBullet->velocity = { realShootDir.x * speed, realShootDir.y * speed, realShootDir.z * speed };
	}

	bullets_.push_back(newBullet);
}


// ==========================================
// 自機がダメージを受けた時の処理
// ==========================================
void Player::OnDamage() {
	// すでに死んでいるか、無敵時間中なら何もしない（ノーダメージ）
	if (isDead_ || invincibilityTimer_ > 0) { return; }

	hp_ -= 1;                    // HPを1減らす
	invincibilityTimer_ = 60;    // 60フレーム（約1秒）無敵にする！

	OutputDebugStringA("Player Took Damage!!!\n");

	// HPが0になったら死亡！
	if (hp_ <= 0) {
		hp_ = 0;
		isDead_ = true;
		OutputDebugStringA("PLAYER DEAD!!! GAME OVER\n");
	}
}


// ==========================================
// チャージパーティクルを生成する関数
// ==========================================
void Player::SpawnChargeParticle(const Vector3& center) {
	ChargeParticle* p = new ChargeParticle();
	p->object3d = new Object3d();
	p->object3d->Initialize(object3dCommon_);
	p->object3d->SetModel("cube.obj"); // 既存のcubeモデルを流用

	//// 中心からランダムな方向・距離に発生させる
	//float angle = (rand() % 360) * 3.141592f / 180.0f;
	//float height = ((rand() % 200) - 100) / 100.0f; // -1.0f ~ 1.0f
	//float radius = 3.0f + ((rand() % 20) / 10.0f);  // 3.0f ~ 5.0f の距離
	float spread = 0.5f;
	float offsetX = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * spread; // -2.0 〜 2.0
	float offsetY = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * spread;
	float offsetZ = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * spread;

	/*p->startPos.x = center.x + std::cos(angle) * radius;
	p->startPos.y = center.y + height * radius;
	p->startPos.z = center.z + std::sin(angle) * radius;*/

	p->startPos.x = center.x + offsetX;
	p->startPos.y = center.y + offsetY;
	p->startPos.z = center.z + offsetZ;

	p->position = p->startPos;
	p->progress = 0.0f;

	// スピードも少しランダムにしてバラつきを出す
	p->speed = 0.02f + ((rand() % 15) / 1000.0f); // 0.02 ~ 0.035

	chargeParticles_.push_back(p);
}

// ==========================================
// チャージパーティクルの更新（中心に引き寄せる）
// ==========================================
void Player::UpdateChargeParticles() {
	// 吸い込まれる中心点（常に最新のプレイヤー位置を追従させる）
	Vector3 center = transform.translate;
	center.y += 1.0f;

	for (auto it = chargeParticles_.begin(); it != chargeParticles_.end(); ) {
		ChargeParticle* p = *it;
		p->progress += p->speed;

		// 進行度が1.0（中心に到達）を超えたか、チャージが中断されたら削除
		if (p->progress >= 1.0f || !isCharging_) {
			delete p->object3d;
			delete p;
			it = chargeParticles_.erase(it);
			continue;
		}

		// 線形補間(Lerp)で現在地を計算：スタート地点から中心地点へ徐々に移動
		p->position.x = p->startPos.x + (center.x - p->startPos.x) * p->progress;
		p->position.y = p->startPos.y + (center.y - p->startPos.y) * p->progress;
		p->position.z = p->startPos.z + (center.z - p->startPos.z) * p->progress;

		// 演出：中心に近づくほど小さくする (0.3f から 0.0f へ)
		float scale = 0.05f * (1.0f - p->progress);

		// チャージ時間が長いほど全体を大きく・激しくしたい場合は chargeTimer_ を掛ける応用も可能です

		p->object3d->SetScale({ scale, scale, scale });
		p->object3d->SetTranslate(p->position);
		p->object3d->SetCamera(camera_);
		p->object3d->Update();

		++it;
	}
}

// ==========================================
// デストラクタｖ
// ==========================================
Player::~Player() {
	// 自機モデルの解放
	delete object3d_;
	object3d_ = nullptr;

	// 画面に残っている弾モデルの解放
	for (Bullet* b : bullets_) {
		delete b->object3d;
		delete b;
	}
	bullets_.clear();

	// ==== Player::~Player() (デストラクタ) 内に追加 ====

	for (ChargeParticle* p : chargeParticles_) {
		delete p->object3d;
		delete p;
	}
	chargeParticles_.clear();
}