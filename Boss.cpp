#include "Boss.h"
#include "Object3dCommon.h"
#include "ModelManager.h" 
#include "Camera.h"
#include "Player.h"
#include "ParticleManager.h"
#include <stdlib.h>
#include <cmath>   


void Boss::Initialize(Object3dCommon* object3dCommon, Camera* camera, Audio* audio) {

	// ==========================================
	// 1. 胴体（Body）の準備
	// ==========================================
	
	ModelManager::GetInstance()->LoadModel("boss.obj");

	objectBody_ = new Object3d();
	objectBody_->Initialize(object3dCommon);

	objectBody_->SetModel("boss.obj");


	// ==========================================, ,
	// 2. 左腕（Left Arm）の準備
	// ==========================================
	ModelManager::GetInstance()->LoadModel("alphaBossLeftArm.obj");

	objectLeftArm_ = new Object3d();
	objectLeftArm_->Initialize(object3dCommon);
	objectLeftArm_->SetModel("alphaBossLeftArm.obj");


	// ==========================================
	// 3. 右腕（Right Arm）の準備
	// ==========================================
	ModelManager::GetInstance()->LoadModel("alphaBossRightArm.obj");

	objectRightArm_ = new Object3d();
	objectRightArm_->Initialize(object3dCommon);
	objectRightArm_->SetModel("alphaBossRightArm.obj");


	// ==========================================
	 // 4. サイズと初期位置の設定
	 // ==========================================
	 
	Vector3 bossScale = { 0.5f, 0.5f, 0.5f };
	objectBody_->SetScale(bossScale_);
	objectLeftArm_->SetScale(bossScale_);
	objectRightArm_->SetScale(bossScale_);

	// ② カメラから少し離れた位置（奥）に置く
	bossPos_ = { 0.0f,150.0f, 10.0f };
	objectBody_->SetTranslate(bossPos_);
	objectLeftArm_->SetTranslate({ bossPos_.x - 0.5f, bossPos_.y, bossPos_.z });
	objectRightArm_->SetTranslate({ bossPos_.x + 0.5f, bossPos_.y, bossPos_.z });
	// ==========================================
	// 5. 向き（回転）の設定
	// ==============================,============

	objectBody_->SetRotate(bossRotate_);
	objectLeftArm_->SetRotate(bossRotate_);
	objectRightArm_->SetRotate(bossRotate_);

	float armOffset = 2.0f;

	// 左腕はマイナス方向、右腕はプラス方向にズラす
	Vector3 leftArmPos = { bossPos_.x - armOffset, bossPos_.y, bossPos_.z };
	Vector3 rightArmPos = { bossPos_.x + armOffset, bossPos_.y, bossPos_.z };

	objectLeftArm_->SetTranslate(leftArmPos);
	objectRightArm_->SetTranslate(rightArmPos);

	objectBody_->SetCamera(camera);
	objectLeftArm_->SetCamera(camera);
	objectRightArm_->SetCamera(camera);


	// ==========================================
	// 6. 衝撃波の準備
	// ==========================================
	
	ModelManager::GetInstance()->LoadModel("shockwave.obj");

	shockwave_ = new Object3d();
	shockwave_->Initialize(object3dCommon);
	shockwave_->SetModel("shockwave.obj");
	shockwave_->SetCamera(camera);


	// ==========================================
	// 7. ミサイルの準備
	// ==========================================
	
	ModelManager::GetInstance()->LoadModel("missile.obj");

	for (int i = 0; i < kMaxMissiles; i++) {
		missiles_[i] = new Object3d();
		missiles_[i]->Initialize(object3dCommon);
		missiles_[i]->SetModel("missile.obj");
		missiles_[i]->SetCamera(camera);
	}

	// ==========================================
	// 8.　爆発（範囲攻撃）の準備
	// ==========================================
	ModelManager::GetInstance()->LoadModel("explosion.obj");

	explosion_ = new Object3d();
	explosion_->Initialize(object3dCommon);
	explosion_->SetModel("explosion.obj");
	explosion_->SetCamera(camera);

	// ==========================================
	// 8.　重力ゾーンの準備
	// ==========================================
	ModelManager::GetInstance()->LoadModel("gravity.obj");

	for (int i = 0; i < kMaxSpheres; i++) {
		spheres_[i] = new Object3d();
		spheres_[i]->Initialize(object3dCommon);
		spheres_[i]->SetModel("gravity.obj");
		spheres_[i]->SetCamera(camera);
	}

	audio_ = audio;
	bossBGM_ = audio_->SoundLoadFile("resources/Overload_Ratio.mp3");
	explosionSound_ = audio_->SoundLoadFile("resources/crash.mp3");
	missileSound_ = audio_->SoundLoadFile("resources/missile.mp3");
	shockWaveSound_ = audio_->SoundLoadFile("resources/shockWave.mp3");

}

void Boss::Update(Player* player) {

	if (isDead_) { return; }

	// ==========================================
	// ボス撃破時の「やられ演出・崩れ落ち」
	// ==========================================
	if (isDying_) {
		deathTimer_++;

		if (deathTimer_ > 240) {
			bossPos_.y -= 0.05f; // ズズズ…と地面に沈んでいく

			// 少し画面を揺らすための小刻みな震え（痙攣）
			bossPos_.x += ((rand() % 10) / 10.0f - 0.5f) * 0.2f;
		}

		// 300フレーム（秒）経ったら、完全に消滅（ゲームクリア！）
		if (deathTimer_ > 300) {
			isDead_ = true;
			isDying_ = false;
			OutputDebugStringA("GAME CLEAR!!!\n");
		}

		float yOffset = 1.0f;

		objectBody_->SetTranslate({ bossPos_.x, bossPos_.y + yOffset, bossPos_.z });
		objectLeftArm_->SetTranslate({ leftArmPos_.x, leftArmPos_.y + yOffset, leftArmPos_.z });
		objectRightArm_->SetTranslate({ rightArmPos_.x, rightArmPos_.y + yOffset, rightArmPos_.z });

		objectBody_->Update();
		objectLeftArm_->Update();
		objectRightArm_->Update();

		return;
	
	}
	// ==========================================
	 // スタン（気絶）中の処理！
	 // ==========================================
	if (stunTimer_ > 0) {
		stunTimer_--;

		// 1. 回転の計算
		float spinSpeed = 0.2f;
		int elapsedFrames = 180 - stunTimer_;
		float currentYaw = elapsedFrames * spinSpeed;

		// 2. 胴体を回して更新
		objectBody_->SetTranslate({ bossPos_.x, bossPos_.y + 1.0f, bossPos_.z });
		objectBody_->SetRotate({ 0.0f, currentYaw, 0.0f });
		objectBody_->Update();


		float radius = 0.5f; // 肩幅（中心からの距離）

		// 左腕の計算（sinとcosで円を描く）
		Vector3 leftPos;
		leftPos.x = bossPos_.x - radius * std::cos(currentYaw);
		leftPos.y = bossPos_.y + 1.0f;
		leftPos.z = bossPos_.z + radius * std::sin(currentYaw);

		objectLeftArm_->SetTranslate(leftPos);
		objectLeftArm_->SetRotate({ 0.0f, currentYaw, 0.0f });
		objectLeftArm_->Update(); // 腕のUpdateを呼ぶ

		// 右腕の計算（左腕の反対側）
		Vector3 rightPos;
		rightPos.x = bossPos_.x + radius * std::cos(currentYaw);
		rightPos.y = bossPos_.y + 1.0f;
		rightPos.z = bossPos_.z - radius * std::sin(currentYaw);

		objectRightArm_->SetTranslate(rightPos);
		objectRightArm_->SetRotate({ 0.0f, currentYaw, 0.0f });
		objectRightArm_->Update(); // 腕のUpdateを呼ぶ

		// 更新が終わったのでここでスキップ
		return;
	}

	// ==========================================
	// 登場演出（ゲーム開始直後）
	// ==========================================
	if (isAppearing_) {

		//指定した時間だけ上空で待機する！
		if (fallDelayTimer_ < 300) {
			fallDelayTimer_++;
		}
		// 待機時間が終わったら、落下開始
		else {
			// 1. 猛スピードで落下
			if (bossPos_.y > 0.0f) {
				bossPos_.y -= 2.0f; // 落下スピード

				// 地面に激突した瞬間！
				if (bossPos_.y <= 0.0f) {
					bossPos_.y = 0.0f;

					//// 着地したら衝撃波！
					//isShockwaveActive_ = true;
					//shockwavePos_ = { bossPos_.x, 0.01f, bossPos_.z }; // 位置はボスの足元

					//if (phase_ == 1) {
					//	shockwaveScale_ = { 20.0f, 0.1f, 2.0f }; // 第1形態：横に広い直線の波
					//} else {
					//	shockwaveScale_ = { 0.1f, 0.1f, 0.1f };  // 第2形態：ここから円状に広がるので初期サイズは極小
					//}

					OutputDebugStringA("BOSS LANDED!!!\n");

					audio_->SoundPlayWave(explosionSound_, false);
				}
			}
			// 2. 着地後、待機（タイマーを進める）
			else {
				appearanceTimer_++;

				// 待ったら、戦闘開始
				if (appearanceTimer_ >= 90) {
					isAppearing_ = false; // 登場状態を解除
					attackTimer_ = 0;     // 攻撃タイマーを0からスタート！
					OutputDebugStringA("BATTLE START!!!\n");

					audio_->SoundPlayWave(bossBGM_, true);
				}
			}
		}
	}





	// ==========================================
	// 形態変化（第1 → 第2）の演出中！
	// ==========================================
	else if (isTransitioning_) {
		transitionTimer_++;

		// 腕を強制的に定位置に戻して待機
		leftArmPos_ = { bossPos_.x - 0.5f, bossPos_.y, bossPos_.z };
		rightArmPos_ = { bossPos_.x + 0.5f, bossPos_.y, bossPos_.z };
		leftPunchState_ = PunchState::kIdle;
		rightPunchState_ = PunchState::kIdle;

		// 120フレーム（2秒）経ったら第2形態スタート！
		if (transitionTimer_ >= 180) {
			phase_ = 2;              // 第2形態へ！
			hp_ = 10;               // 第2形態のHP！
			attackTimer_ = 0;        // 攻撃タイマーリセット
			isTransitioning_ = false;// 演出終了
			jumpCount_ = 0;          // ジャンプ回数リセット
			OutputDebugStringA("BOSS PHASE 2 START!!!\n");
		}
	}

	// ==========================================
	// ② 第1形態の動き（今までの自機狙いロケットパンチ）
	// ==========================================
	else if (phase_ == 1) {
		if (!isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_ &&
			leftPunchState_ == PunchState::kIdle && rightPunchState_ == PunchState::kIdle) {
			attackTimer_++;
		}

		// 左腕パンチ
		if (attackTimer_ == 60 && leftPunchState_ == PunchState::kIdle) {
			leftPunchState_ = PunchState::kPunch;
			if (player) {
				Vector3 pPos = player->GetTranslate();
				Vector3 targetPos = { pPos.x, pPos.y, pPos.z };
				float dx = targetPos.x - leftArmPos_.x; float dy = targetPos.y - leftArmPos_.y; float dz = targetPos.z - leftArmPos_.z;
				float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
				float speed = 0.5f;
				if (distance > 0.0f) {
					leftArmVelocity_ = { (dx / distance) * speed, (dy / distance) * speed, (dz / distance) * speed };
				}
			}
		}
		// 右腕パンチ
		if (attackTimer_ == 120 && rightPunchState_ == PunchState::kIdle) {
			rightPunchState_ = PunchState::kPunch;
			if (player) {
				Vector3 pPos = player->GetTranslate();
				Vector3 targetPos = { pPos.x, pPos.y, pPos.z };
				float dx = targetPos.x - rightArmPos_.x; float dy = targetPos.y - rightArmPos_.y; float dz = targetPos.z - rightArmPos_.z;
				float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
				float speed = 0.5f;
				if (distance > 0.0f) {
					rightArmVelocity_ = { (dx / distance) * speed, (dy / distance) * speed, (dz / distance) * speed };
				}
			}
		}
		// 大技（ジャンプ＆衝撃波）
		if (attackTimer_ >= 180 && leftPunchState_ == PunchState::kIdle && rightPunchState_ == PunchState::kIdle && !isMovingToEdge_ && !isJumping_ && !isShockwaveActive_) {
			isMovingToEdge_ = true;
		}
	}
	// ==========================================
	// 第2形態の動き（暴走モード：連続ジャンプ衝撃波）
	// ==========================================
	else if (phase_ == 2) {
		// 攻撃のテンポを管理
		if (!isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_) {
			attackTimer_++;
		}

		// ==========================================
		//  攻撃0：ホーミングミサイル発射！
		// ==========================================
		if (attackTimer_ == 10) { // ジャンプ攻撃の少し前に撃つ！
			for (int i = 0; i < kMaxMissiles; i++) {
				isMissileActive_[i] = true;
				missileHomingTimer_[i] = 75; // ホーミングする

				// 発射位置：ボスの少し上、左右に振り分ける
				float offsetX = -3.0f + (i * 2.0f); // iが増えるごとに右にズレる
				missilePos_[i] = { bossPos_.x + offsetX, bossPos_.y + 3.0f, bossPos_.z };

				// 最初の初速：上にフワッと撃ち上げる
				float spreadX = (offsetX * 0.1f); // 外側に広がる力
				missileVelocity_[i] = { spreadX, 0.4f, -0.1f };
			}

			audio_->SoundPlayWave(missileSound_, false);
		}

		//時差ミサイルにする場合
		//// メインの時計(attackTimer_)が 10 になったら、独立タイマーのスイッチを「ON(1)」にする！
		//if (attackTimer_ == 10 && !isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_) {
		//    missileSequenceTimer_ = 1;
		//}

		//// 独立タイマーが動いている間は、ボスがジャンプ中だろうと関係なく進み続ける！
		//if (missileSequenceTimer_ > 0) {
		//    missileSequenceTimer_++; // 専用時計の針を進める

		//    for (int i = 0; i < kMaxMissiles; i++) {
		//        // 発射タイミング（タイマーが 2, 22, 42, 62... になったら発射）
		//        int fireTime = 2 + (i * 20);

		//        // ★ attackTimer_ ではなく missileSequenceTimer_ で判定する！
		//        if (missileSequenceTimer_ == fireTime) {
		//            isMissileActive_[i] = true;
		//            missileHomingTimer_[i] = 75;

		//            float direction = (i % 2 == 0) ? -1.0f : 1.0f;
		//            float spread = 3.0f + (i / 2) * 1.5f;
		//            float offsetX = direction * spread;

		//            missilePos_[i] = { bossPos_.x + offsetX, bossPos_.y + 3.0f, bossPos_.z };
		//            missileVelocity_[i] = { direction * 0.3f, 0.4f, -0.1f };
		//        }
		//    }

		//    // 全部撃ち終わる十分な時間が経ったら、スイッチを「OFF(0)」に戻す
		//    if (missileSequenceTimer_ > 100) {
		//        missileSequenceTimer_ = 0;
		//    }
		//}

		// 攻撃1：連続ジャンプ衝撃波（3連続）
		if (attackTimer_ == 60 && !isMovingToEdge_ && !isJumping_ && !isShockwaveActive_ && !isReturningToCenter_) {
			isMovingToEdge_ = true;
			jumpCount_ = 0; // ジャンプ回数をリセット
		}

		// ==========================================
		// 攻撃2：ブラックホール（プレイヤーを吸引）
		// ==========================================
		// タイマーが150〜450の間（約5秒間）、強烈に吸い寄せる
		if (attackTimer_ >= 180 && attackTimer_ < 450) {
			isSuctionActive_ = true;

			if (player) {
				Vector3 pPos = player->GetTranslate();
				float dx = bossPos_.x - pPos.x;
				float dz = bossPos_.z - pPos.z; // 足元に吸い寄せるのでY（高さ）は無視
				float dist = std::sqrt(dx * dx + dz * dz);

				if (dist > 0.1f) {
				
					float suctionPower = 0.12f;
					pPos.x += (dx / dist) * suctionPower;
					pPos.z += (dz / dist) * suctionPower;

					// ボス側からプレイヤーの座標を上書きして強制移動させる！
					player->SetTranslate(pPos);
				}
			}
		} else {
			isSuctionActive_ = false;
		}

		// ==========================================
		// 攻撃3：大爆発（足元範囲ドカン！）
		// ==========================================
		// 吸引が終わった瞬間（450）に爆発スタート！
		if (attackTimer_ == 450) {
			isExplosionActive_ = true;
			explosionScale_ = { 0.1f, 0.1f, 0.1f };

			audio_->SoundPlayWave(explosionSound_, false);
		}

		if (isExplosionActive_) {
			// 猛スピードで巨大化していく！
			explosionScale_.x += 0.5f;
			explosionScale_.z += 0.5f;
			explosionScale_.y = 1.0f; // 平面なので高さは固定

			// 最大サイズ（15.0f）まで広がったら終了
			if (explosionScale_.x > 15.0f) {
				isExplosionActive_ = false;
				
				//attackTimer_ = -120;
			}

			explosion_->SetTranslate({ bossPos_.x, 0.01f, bossPos_.z }); // ボスの足元
			explosion_->SetScale(explosionScale_);
			explosion_->Update();
		}

	// ==========================================
	// 攻撃4：グラビティ・スフィア生成
	// ==========================================
	// 全てのスフィアが非アクティブかチェック
		bool anyActive = false;
		for (int i = 0; i < kMaxSpheres; i++) { if (isSphereActive_[i]) anyActive = true; }

		
		if (!anyActive && attackTimer_ == 500) {
			for (int i = 0; i < kMaxSpheres; i++) {
				isSphereActive_[i] = true;
				sphereLifeTimer_[i] = 300;

				// 生成位置をプレイヤーの周りにセット
				float rx = ((rand() % 100) / 100.0f - 0.5f) * 20.0f;
				float rz = ((rand() % 100) / 100.0f - 0.5f) * 20.0f;
				spherePos_[i] = { player->GetTranslate().x + rx, 0.01f, player->GetTranslate().z + rz };

				OutputDebugStringA("GRAVITY SPHERE GENERATED!!!\n");
			}

			attackTimer_ = -120;
		}

		// スフィアの寿命管理
		for (int i = 0; i < kMaxSpheres; i++) {
			if (isSphereActive_[i]) {
				sphereLifeTimer_[i]--;
				if (sphereLifeTimer_[i] <= 0) {
					isSphereActive_[i] = false;
				} else {
					float domeY = -0.2f;
					spheres_[i]->SetTranslate(spherePos_[i]);
					spheres_[i]->SetScale({ 1.0f, 1.0f, 1.0f }); // 範囲に合わせて大きく
					Vector4 domeColor = { 0.7f, 0.0f, 1.0f, 0.4f }; // 鮮やかな紫、透明度40%
					
					spheres_[i]->Update();
				}
			}
		}

	}

	// ==========================================
	  // 全形態共通：腕の移動処理
	  // ==========================================
	// ==========================================
	// 自機の方を向く処理
	// ==========================================
	if (player && stunTimer_ <= 0) {
		Vector3 pPos = player->GetTranslate();
		float dx = pPos.x - bossPos_.x;
		float dz = pPos.z - bossPos_.z;
		bossRotate_.y = std::atan2(dx, dz);
	}

	// ==========================================
	// 全形態共通：腕の移動処理
	// ==========================================
	// 肩の位置をボスの回転に合わせて計算
	float cosY = std::cos(bossRotate_.y);
	float sinY = std::sin(bossRotate_.y);
	float shoulderOffsetX = cosY * 2.5f;
	float shoulderOffsetZ = -sinY * 2.5f; // ※もし腕が前後にズレる場合は +sinY に直してください

	Vector3 leftShoulder = { bossPos_.x - shoulderOffsetX, bossPos_.y, bossPos_.z - shoulderOffsetZ };
	switch (leftPunchState_) {
	case PunchState::kIdle: leftArmPos_ = leftShoulder; break;
	case PunchState::kPunch:
		leftArmPos_.x += leftArmVelocity_.x; leftArmPos_.y += leftArmVelocity_.y; leftArmPos_.z += leftArmVelocity_.z;
		leftPunchTimer_++;
		if (leftPunchTimer_ >= 100) { leftPunchState_ = PunchState::kReturn; leftPunchTimer_ = 0; }
		break;
	case PunchState::kReturn:
	{
		float dx = leftShoulder.x - leftArmPos_.x; float dy = leftShoulder.y - leftArmPos_.y; float dz = leftShoulder.z - leftArmPos_.z;
		float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
		if (dist > 0.5f) { leftArmPos_.x += (dx / dist) * 0.5f; leftArmPos_.y += (dy / dist) * 0.5f; leftArmPos_.z += (dz / dist) * 0.5f; } else { leftArmPos_ = leftShoulder; leftPunchState_ = PunchState::kIdle; }
		break;
	}
	case PunchState::kReflected:
	{   // 弾き返された時のホーミング
		Vector3 targetPos = { bossPos_.x, bossPos_.y + 1.0f, bossPos_.z };
		float dx = targetPos.x - leftArmPos_.x; float dy = targetPos.y - leftArmPos_.y; float dz = targetPos.z - leftArmPos_.z;
		float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

		if (dist > 1.0f) {
			float reflectSpeed = 1.5f; // パンチの3倍の速度
			leftArmPos_.x += (dx / dist) * reflectSpeed; leftArmPos_.y += (dy / dist) * reflectSpeed; leftArmPos_.z += (dz / dist) * reflectSpeed;
		} else {
			leftPunchState_ = PunchState::kIdle;
			leftArmPos_ = leftShoulder;
			stunTimer_ = 180; // ボスをスタンさせる
			OutputDebugStringA("BOSS STUNNED!!!\n");
		}
		break;
	}
	}

	Vector3 rightShoulder = { bossPos_.x + shoulderOffsetX, bossPos_.y, bossPos_.z + shoulderOffsetZ };
	switch (rightPunchState_) {
	case PunchState::kIdle: rightArmPos_ = rightShoulder; break;
	case PunchState::kPunch:
		rightArmPos_.x += rightArmVelocity_.x; rightArmPos_.y += rightArmVelocity_.y; rightArmPos_.z += rightArmVelocity_.z;
		rightPunchTimer_++;
		if (rightPunchTimer_ >= 100) { rightPunchState_ = PunchState::kReturn; rightPunchTimer_ = 0; }
		break;
	case PunchState::kReturn:
	{
		float dx = rightShoulder.x - rightArmPos_.x; float dy = rightShoulder.y - rightArmPos_.y; float dz = rightShoulder.z - rightArmPos_.z;
		float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
		if (dist > 0.5f) { rightArmPos_.x += (dx / dist) * 0.5f; rightArmPos_.y += (dy / dist) * 0.5f; rightArmPos_.z += (dz / dist) * 0.5f; } else { rightArmPos_ = rightShoulder; rightPunchState_ = PunchState::kIdle; }
		break;
	}
	case PunchState::kReflected:
	{   // 弾き返された時のホーミング
		Vector3 targetPos = { bossPos_.x, bossPos_.y + 1.0f, bossPos_.z };
		float dx = targetPos.x - rightArmPos_.x; float dy = targetPos.y - rightArmPos_.y; float dz = targetPos.z - rightArmPos_.z;
		float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

		if (dist > 1.0f) {
			float reflectSpeed = 1.5f;
			rightArmPos_.x += (dx / dist) * reflectSpeed; rightArmPos_.y += (dy / dist) * reflectSpeed; rightArmPos_.z += (dz / dist) * reflectSpeed;
		} else {
			rightPunchState_ = PunchState::kIdle;
			rightArmPos_ = rightShoulder;
			stunTimer_ = 180;
			OutputDebugStringA("BOSS STUNNED!!!\n");
		}
		break;
	}
	}

	// ==========================================
	// 全形態共通：大技（ジャンプ＆衝撃波）の処理
	// ==========================================
	if (isMovingToEdge_) {
		bossPos_.z += (phase_ == 2) ? 0.4f : 0.2f; // 第2形態は下がるのも速い
		if (bossPos_.z >= 15.0f) {
			bossPos_.z = 15.0f;
			isMovingToEdge_ = false;
			isJumping_ = true;
			velocityY_ = (phase_ == 2) ? 0.6f : 0.6f; // 第2形態は初回大ジャンプ
		}
	}
	if (isJumping_) {
		bossPos_.y += velocityY_;
		velocityY_ -= 0.05f; // 重力

		if (bossPos_.y <= 0.0f) {
			bossPos_.y = 0.0f;
			isJumping_ = false;

			if (phase_ == 1) {
				attackTimer_ = 0; // 第1形態はここで攻撃終了
			} else {
				jumpCount_++; // 第2形態はジャンプ回数をカウント
			}

			// 着地したら衝撃波！
			isShockwaveActive_ = true;

			// 地面に埋もれないように 0.5f 浮かせた位置からスタート
			shockwavePos_ = { bossPos_.x, 0.5f, bossPos_.z };

			// 🌟 飛ぶ向きと画像の回転を、着地した瞬間のボスの向きに合わせる！
			shockwaveRotate_ = bossRotate_;
			float waveSpeed = 0.4f; // 衝撃波の飛ぶスピード
			// サインとコサインを使って、向いている方向へのベクトルを作る
			shockwaveVelocity_.x = std::sin(bossRotate_.y) * waveSpeed;
			shockwaveVelocity_.y = 0.0f;
			shockwaveVelocity_.z = std::cos(bossRotate_.y) * waveSpeed;

			// 形態によって「最初の形」を分ける！
			if (phase_ == 1) {
				shockwaveScale_ = { 20.0f, 1.0f, 2.0f }; // 第1形態：今まで通りの横長
			} else if (phase_ == 2) {
				shockwaveScale_ = { 0.1f, 1.0f, 0.1f };  // 第2形態：小さな「真円」からスタート！
			}

			audio_->SoundPlayWave(shockWaveSound_, false);
		}
	}
	
	if (isShockwaveActive_) {
		if (phase_ == 1) {
			// ==========================================
			// 第1形態：ボスの向いていた方向へ進む直線衝撃波
			// ==========================================
			// 🌟 XとZ両方にスピードを足して斜めにも進めるようにする
			shockwavePos_.x += shockwaveVelocity_.x;
			shockwavePos_.z += shockwaveVelocity_.z;

			// 飛んだ距離を測って、40.0f以上離れたら画面外として消す
			float dx = shockwavePos_.x - bossPos_.x;
			float dz = shockwavePos_.z - bossPos_.z;
			if (std::sqrt(dx * dx + dz * dz) > 40.0f) {
				isShockwaveActive_ = false;
				isReturningToCenter_ = true;
			}
		} else if (phase_ == 2) {
			// ==========================================
			// 第2形態：その場から円状に広がる衝撃波
			// ==========================================
			// 🌟 修正1：xとzの速度を同じ(0.4f)にして「真円」にする！（これで当たり判定とピッタリ合う）
			shockwaveScale_.x += 0.4f;
			shockwaveScale_.z += 0.4f;
			shockwaveScale_.y = 1.0f;

			if (shockwaveScale_.x > 40.0f) {
				isShockwaveActive_ = false;

				// 🌟 修正2：消滅した瞬間に、次のジャンプまでの「間隔（ディレイ）」をセット
				if (jumpCount_ < 3) {
					jumpDelayTimer_ = 60; // 60フレーム（約1秒）待機する！ここを増減すると間隔が変わる
				} else {
					isReturningToCenter_ = true; // 3回終わったら戻る
				}
			}
		}

		shockwave_->SetTranslate(shockwavePos_);
		shockwave_->SetScale(shockwaveScale_);
		shockwave_->SetRotate(shockwaveRotate_);
		shockwave_->Update();
	}

	// ジャンプの間隔（ディレイ）をカウントして、0になったら次のジャンプ開始
	if (jumpDelayTimer_ > 0) {
		jumpDelayTimer_--;
		if (jumpDelayTimer_ <= 0) {
			isJumping_ = true;
			velocityY_ = 0.4f; // 次のジャンプの高さ
		}
	}

	if (isReturningToCenter_) {
		bossPos_.z -= (phase_ == 2) ? 0.4f : 0.2f; // 戻るのも速い
		if (bossPos_.z <= 10.0f) {
			bossPos_.z = 10.0f;
			isReturningToCenter_ = false;
			attackTimer_ = (phase_ == 2) ? 100 : 0; // 第2形態は少し休む
		}
	}

	// ==========================================
	// ミサイルの移動とホーミング処理
	// ==========================================
	for (int i = 0; i < kMaxMissiles; i++) {
		if (isMissileActive_[i]) {

			// 寿命が残っている間だけ、プレイヤーの方へ曲がる（ホーミング）
			if (missileHomingTimer_[i] > 0) {
				missileHomingTimer_[i]--; // タイマーを減らす

				if (player) {
					Vector3 pPos = player->GetTranslate();
					Vector3 targetPos = { pPos.x, pPos.y + 1.0f, pPos.z };

					float dx = targetPos.x - missilePos_[i].x;
					float dy = targetPos.y - missilePos_[i].y;
					float dz = targetPos.z - missilePos_[i].z;
					float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

					if (distance > 0.0f) {
						float speed = 0.4f; // ミサイルの最高速度
						Vector3 desiredVel = { (dx / distance) * speed, (dy / distance) * speed, (dz / distance) * speed };


						missileVelocity_[i].x = (missileVelocity_[i].x * 0.95f) + (desiredVel.x * 0.05f);
						missileVelocity_[i].y = (missileVelocity_[i].y * 0.95f) + (desiredVel.y * 0.05f);
						missileVelocity_[i].z = (missileVelocity_[i].z * 0.95f) + (desiredVel.z * 0.05f);
					}
				}
			}
			// ※ タイマーが0になったら上の if文 を無視するので、速度(Velocity)が更新されず、
			//    そのままの角度で「直進」し続ける（＝ホーミング切れ）

			// 速度を足して移動させる
			missilePos_[i].x += missileVelocity_[i].x;
			missilePos_[i].y += missileVelocity_[i].y;
			missilePos_[i].z += missileVelocity_[i].z;

			// 2.進行方向を向かせるための回転計算
			// atan2(x, z) で、XZ平面上の進んでいる角度を求めます
			float angleY = std::atan2(missileVelocity_[i].x, missileVelocity_[i].z);

			// 上下方向（ピッチ）の角度も計算（y と 平面の速度から算出）
			float velocityXZ = std::sqrt(missileVelocity_[i].x * missileVelocity_[i].x + missileVelocity_[i].z * missileVelocity_[i].z);
			float angleX = std::atan2(-missileVelocity_[i].y, velocityXZ);

			// 地面に当たるか、画面のずっと奥/手前に行ったら消滅
			if (missilePos_[i].y <= 0.0f || missilePos_[i].z < -30.0f || missilePos_[i].z > 30.0f) {
				isMissileActive_[i] = false;
			} else {
				// 3Dオブジェクトに座標をセットして更新
				missiles_[i]->SetTranslate(missilePos_[i]);
				missiles_[i]->SetScale({ 0.2f, 0.2f, 0.2f }); // 弾のサイズ
				missiles_[i]->SetRotate({ angleX, angleY, 0.0f });
				missiles_[i]->Update();
			}
		}
	}

	// ==========================================
	// 常にプレイヤーの方向を向かせる処理
	// ==========================================
	// 1. プレイヤーの座標を取得
	Vector3 pPos = player->GetTranslate(); // ※関数名はPlayerクラスに合わせてください

	// 2. ボスからプレイヤーへの距離（XとZの差分）を計算
	float dx = pPos.x - bossPos_.x;
	float dz = pPos.z - bossPos_.z;

	// 3. atan2で角度を計算し、ボスのY軸回転に代入
	bossRotate_.y = std::atan2(dx, dz);
	// ==========================================


	// ⬇️ この下は元々書いてあるコードです
	objectBody_->SetTranslate(bossPos_);
	objectBody_->SetRotate(bossRotate_);

	// ==========================================
	// 最後に座標をセット
	// ==========================================

	float yOffset = 1.0f;
	float shakeX = 0.0f;
	float shakeY = 0.0f;

	// 形態変化中なら、ボス自身を震わせる！
	if (isTransitioning_) {
		if (transitionTimer_ < 60) {
			// 前半の1秒：小刻みにプルプル震える
			shakeX = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
			shakeY = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
		} else {
			// 後半の2秒：激しく震える（暴走）
			shakeX = ((rand() % 100) / 100.0f - 0.5f) * 0.4f;
			shakeY = ((rand() % 100) / 100.0f - 0.5f) * 0.4f;
		}
	}

	// ==========================================
		// 吸引中にボスを巨大化させる演出
		// ==========================================
	Vector3 finalVisualScale = bossScale_; // 基本の大きさをコピー

	if (isSuctionActive_) {
		// 吸引開始(150)から終了(450)までの300フレームで変化させる
		float t = (float)(attackTimer_ - 150) / 300.0f;

		// 1.0(元のサイズ) から 2.0(2倍) まで徐々に大きくする
		float growFactor = 1.0f + t * 1.0f;

		finalVisualScale.x *= growFactor;
		finalVisualScale.y *= growFactor;
		finalVisualScale.z *= growFactor;
	}
	// attackTimer_ が 450 になり爆発(isExplosionActive_)が始まると、
	// isSuctionActive_ は false になるため、自動的に元の bossScale_ に戻ります。

	objectBody_->SetScale(finalVisualScale);
	objectLeftArm_->SetScale(finalVisualScale);
	objectRightArm_->SetScale(finalVisualScale);

	
	objectBody_->SetTranslate({ bossPos_.x + shakeX, bossPos_.y + yOffset + shakeY, bossPos_.z });
	objectLeftArm_->SetTranslate({ leftArmPos_.x + shakeX, leftArmPos_.y + yOffset + shakeY, leftArmPos_.z });
	objectRightArm_->SetTranslate({ rightArmPos_.x + shakeX, rightArmPos_.y + yOffset + shakeY, rightArmPos_.z });

	// スタン中（腕をぐるぐる回している時）以外は、基本の回転をセットする
	if (stunTimer_ <= 0) {
		objectBody_->SetRotate(bossRotate_);
		objectLeftArm_->SetRotate(bossRotate_);
		objectRightArm_->SetRotate(bossRotate_);
	}

	objectBody_->Update();
	objectLeftArm_->Update();
	objectRightArm_->Update();

	return;
}

void Boss::Draw() {

	if (isDead_) { return; }
	if (objectBody_) { objectBody_->Draw(); }


	if (phase_ == 1) {
		if (objectLeftArm_) { objectLeftArm_->Draw(); }
		if (objectRightArm_) { objectRightArm_->Draw(); }
	}

	if (isShockwaveActive_ && shockwave_) {
		shockwave_->Draw();
	}

	for (int i = 0; i < kMaxMissiles; i++) {
		if (isMissileActive_[i] && missiles_[i]) {
			missiles_[i]->Draw();
		}
	}

	if (isExplosionActive_ && explosion_) {
		explosion_->Draw();
	}

	for (int i = 0; i < kMaxSpheres; i++) {
		if (isSphereActive_[i] && spheres_[i]) {
			spheres_[i]->Draw();
		}
	}
}

Boss::~Boss() {
	delete objectBody_;
	delete objectLeftArm_;
	delete objectRightArm_;
	delete shockwave_;


	for (int i = 0; i < kMaxMissiles; i++) {
		delete missiles_[i];
	}

	delete explosion_;

	for (int i = 0; i < kMaxSpheres; i++) {
		if (spheres_[i]) {
			delete spheres_[i];
			spheres_[i] = nullptr;
		}
	}

	audio_->SoundUnload(&bossBGM_);
	audio_->SoundUnload(&explosionSound_);
	audio_->SoundUnload(&missileSound_);
	audio_->SoundUnload(&shockWaveSound_);
}

