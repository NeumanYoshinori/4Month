#include "Game.h"
using namespace MathFunction;

Game::Game()
{
}

Game::~Game()
{
}

void Game::Initialize()
{
	// WindowsAPIの初期化
	winApp_ = new WinApp();
	winApp_->Initialize();

	// DirectXの初期化
	dxBase_ = new DirectXBase();
	dxBase_->Initialize(winApp_);

	// 入力の初期化
	input_ = Input::GetInstance();
	input_->Initialize(winApp_);

	// SRVマネージャの初期化
	//srvManager_ = new SrvManager();
	srvManager_ = SrvManager::GetInstance();
	srvManager_->Initialize(dxBase_);

	// テクスチャマネジャー
	textureManager_ = TextureManager::GetInstance();

	// テクスチャマネージャの初期化
	textureManager_->Initialize(dxBase_, srvManager_);

	// テクスチャを読み込む
	textureManager_->LoadTexture("resources/uvChecker.png");
	textureManager_->LoadTexture("resources/monsterBall.png");

	// スプライト共通部の初期化
	spriteCommon_ = new SpriteCommon();
	spriteCommon_->Initialize(dxBase_);

	// モデルマネージャー
	modelManager_ = ModelManager::GetInstance();

	// 3Dモデルマネージャの初期化
	modelManager_->Initialize(dxBase_);

	// 3Dオブジェクト共通部の初期化
	object3dCommon_ = new Object3dCommon();
	object3dCommon_->Initialize(dxBase_);
	Vector3 rotate[2] = { 0.0f };

	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("field.obj");
	ModelManager::GetInstance()->LoadModel("cube.obj");

	// 3dオブジェクトの初期化
	//Object3d* field = new Object3d();
	//field->Initialize(object3dCommon);

	//// 初期化済みの3Dオブジェクトにモデルを紐づける
	//field->SetModel("field.obj");
	//field->SetRotate({ 85.0f, 0.0f, 0.0f });
	//field->SetTranslate({ 0.0f, 0.0f, 0.0f });

	// カメラの初期化
	camera_ = new Camera();
	camera_->SetRotate({ 0.3f, 0.0f, 0.0f });
	camera_->SetTranslate({ 0.0f, 10.0f, -30.0f });
	//field->SetCamera(camera_);

	sceneManager_ = SceneManager::GetInstance();
	sceneManager_->Initialize(object3dCommon_, camera_, spriteCommon_);

	SceneFactory* sceneFactory = new SceneFactory();
	sceneManager_->SetSceneFactory(sceneFactory);
	sceneManager_->ChangeScene("TITLE");

	// player
	player_ = new Player();
	player_->Initialize(object3dCommon_);
	// playerにモデルを紐づける
	player_->SetModel("player.obj");
	// playerにカメラを紐づける
	player_->SetCamera(camera_);

	// ==========================================
	// ★ 追加：GameScene（ボスと背景の管理者）を作る
	// ==========================================
	/*gameScene_ = new GameScene();
	gameScene_->Initialize(object3dCommon_, camera_);*/


	// パーティクルマネージャ
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Initialize(dxBase_, srvManager_, camera_);

	particleManager_->CreateParticleGroup("circle", "resources/circle.png");

	Transform particleTransform;
	particleTransform.translate = { 0.0f, 0.0f, 0.0f };
	ParticleEmitter* particleEmitter = new ParticleEmitter("circle", particleTransform, 30, 1.0f);

	Transform uvTransformSprite{
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
	};

	// Δtを設定
	const float kDeltaTime = 1.0f / 60.0f;

	//// xAudio
	//ComPtr<IXAudio2> xAudio2;
	//IXAudio2MasteringVoice* masterVoice;

	//HRESULT result;

	//// XAudioエンジンのインスタンスを生成
	//result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//// マスターボイスを生成
	//result = xAudio2->CreateMasteringVoice(&masterVoice);

	//// 音声読み込み
	////SoundData soundData1 = SoundLoadWave("resources/Alarm01.wav");
	//// 音声再生
	////SoundPlayWave(xAudio2.Get(), soundData1);

	//// ブレンドモード
	//static int currentBlend = kBlendModeNone;
	//const char* blendMode[] = { "kBlendModeNone", "kBlendModeNormal", "kBlendModeAdd", "kBlendModeSubtract", "kBlendModeMultiply", "kBlendModeScreen" };

	//// パーティクルが動くか
	//uint32_t canUpdate = false;

	//// コマンドリストを生成する
	//ComPtr<ID3D12GraphicsCommandList> commandList = dxBase->GetCommandList();

}

void Game::Update()
{

	input_->Update();


	// シーンマネージャの更新処理
	if (sceneManager_) {
		sceneManager_->Update(player_);
	}

	// Windowsのメッセージ処理
	if (winApp_->ProcessMessage()) {
		// ゲームループを抜ける
		return;
	}

	// ゲームの処理
	//ImGui_ImplDX12_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	//// 開発用UIの処理
	//ImGui::ShowDemoWindow();

	// キー入力の更新
	player_->Update(input_);
	// 0キーを押したときコンソールにHit 0と表示する
	if (input_->ReleaseKey(DIK_0)) {
		OutputDebugStringA("Hit 0\n");
	}

	if (input_->TriggerKey(DIK_ESCAPE)) { // または PushKey(DIK_ESCAPE)
		return; // ループを抜けて終了処理へ向かう
	}
	// カメラの更新
	camera_->Update();
	//rotation.x += 0.01f;
	// フィールドの更新
	//field->Update();
	//field->SetRotate(rotation);

	// 開発用UIの処理
	//ImGui::ShowDemoWindow();

	// ImGuiの内部コマンドを生成する
	//ImGui::Render();

	//// 描画前処理
	//dxBase_->PreDraw();

	//srvManager_->PreDraw();

	//// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
	//object3dCommon_->DrawSetting();

	//for (uint32_t i = 0; i < 2; i++) {
	//	// 3Dオブジェクトの描画
	//	//object3d[i]->Draw();
	//}

	//player_->Draw();

	////particleManager->Draw();
	//// 3Dオブジェクトの描画
	////field->Draw();

	//gameScene_->Draw();

	//// 共通描画設定
	//spriteCommon_->DrawSetting();

	//// 実際のcommandListのImGuiの描画コマンドを積む
	////ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

	//// 描画後処理
	//dxBase_->PostDraw();

	 // 仮のuvTransformSprite構造体を定義（本来は適切な場所で定義・初期化すること）
	//struct {
	//	Vector3 scale{1.0f, 1.0f, 1.0f};
	//	Vector3 rotate{0.0f, 0.0f, 0.0f};
	//	Vector3 translate{0.0f, 0.0f, 0.0f};
	//} uvTransformSprite;
	//Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
	//uvTransformMatrix = MathFunction::Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
	//uvTransformMatrix = MathFunction::Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
	///*materialData->uvTransform = uvTransformMatrix;*/
}

void Game::Draw()
{
	// 描画前処理
	dxBase_->PreDraw();

	srvManager_->PreDraw();

	// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
	object3dCommon_->DrawSetting();
	
	// シーンマネージャの描画
	if (sceneManager_) {
		sceneManager_->Draw();
	}

	for (uint32_t i = 0; i < 2; i++) {
		// 3Dオブジェクトの描画
		//object3d[i]->Draw();
	}

	player_->Draw();

	//particleManager->Draw();
	// 3Dオブジェクトの描画
	//field->Draw();

	//gameScene_->Draw();

	// 共通描画設定
	spriteCommon_->DrawSetting();

	// 実際のcommandListのImGuiの描画コマンドを積む
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

	// 描画後処理
	dxBase_->PostDraw();
}

void Game::Finalize()
{
	// シーンマネージャの解放
	if (sceneManager_) {
		sceneManager_->Finalize();
	}

	delete winApp_;
	delete dxBase_;
	delete input_;
	//delete srvManager_;
	delete spriteCommon_;
	delete object3dCommon_;
	delete camera_;
	delete player_;
}