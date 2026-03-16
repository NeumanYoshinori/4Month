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
}

void Player::Update(Input* input) {

	if (input) {
		float speed = 0.1f; // 移動速度（必要に応じて調整してください）

		// Wキーで奥(Zプラス)へ移動
		if (input->PushKey(DIK_W)) {
			transform.translate.z += speed;
		}
		// Sキーで手前(Zマイナス)へ移動
		if (input->PushKey(DIK_S)) {
			transform.translate.z -= speed;
		}
		// Aキーで左(Xマイナス)へ移動
		if (input->PushKey(DIK_A)) {
			transform.translate.x -= speed;
		}
		// Dキーで右(Xプラス)へ移動
		if (input->PushKey(DIK_D)) {
			transform.translate.x += speed;
		}
	}

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
