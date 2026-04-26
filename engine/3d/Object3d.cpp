#include "Object3d.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"

using namespace std;
using namespace MathFunction;

void Object3d::Initialize(Object3dCommon* object3dCommon) {
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

	// カメラデータ作成
	CreateCameraData();

	// ポイントライト作成
	CreatePointLight();
}

void Object3d::Update() {
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	} else {
		worldViewProjectionMatrix = worldMatrix;
	}

	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;

	pointLightData->position = transform.translate;
}

void Object3d::Draw() {
	// コマンドリストを作成
	commandList = dxBase_->GetCommandList();

	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	// 平行光源CBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	// カメラのCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	// ポイントライトのCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());

	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}
}

void Object3d::SetModel(const std::string& filePath) {
	// モデルを検索
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::DebugDirectionalLight() {
#ifdef USE_IMGUI
	// 開発用UIの処理
	ImGui::ColorEdit3("DirLightColor", &directionalLightData->color.x);
	ImGui::DragFloat3("DirLightDirection", &directionalLightData->direction.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat("DirIntensity", &directionalLightData->intensity, 0.01f);
#endif
}

void Object3d::DebugPointLight() {
#ifdef USE_IMGUI
	// 開発用UIの処理
	ImGui::ColorEdit3("PointLightColor", &pointLightData->color.x);
	ImGui::DragFloat3("PointLightPosition", &pointLightData->position.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat("PointIntensity", &pointLightData->intensity, 0.01f);
#endif
}

void Object3d::CreateTransformationMatrixData() {
	// TransformationMatrix用のリソースを作る。
	transformationMatrixResource = dxBase_->CreateBufferResource(sizeof(TransformationMatrix));

	// 書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}

void Object3d::CreateDirectionalLight() {
	// 平行光源リソースを作る
	directionalLightResource = dxBase_->CreateBufferResource(sizeof(DirectionalLight));

	// 平行光源リソースにデータを書き込むためのアドレスを取得して平行光源構造体のポインタに割り当てる
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = Normalize({ 0.0f, 1.0f, 0.0f });
	directionalLightData->intensity = 0.0f;
}

void Object3d::CreateCameraData() {
	// カメラリソースを作る
	cameraResource = dxBase_->CreateBufferResource(sizeof(CameraForGPU));

	// 書き込むためのアドレスを作る
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	if (camera_) {
		cameraData->worldPosition = camera_->GetTranslate();
	}
}

void Object3d::CreatePointLight() {
	// ポイントライトリソースを作る
	pointLightResource = dxBase_->CreateBufferResource(sizeof(PointLight));

	// 書き込むためのアドレスを作る
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));

	// デフォルト値はとりあえず以下のようにしておく
	pointLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointLightData->position = { 0.0f, 2.0f, 0.0f };
	pointLightData->intensity = 1.0f;
	pointLightData->radius = 5.0f;
	pointLightData->decay = 1.0f;
}
