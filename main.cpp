#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>
#include <dbghelp.h>
#include <strsafe.h>
#include <vector>
#include <wrl.h>
#include <xaudio2.h>
#include <random>
#include "Input.h"
#include "WinApp.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "MathFunction.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "SrvManager.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "Player.h"
#include "GameScene.h"
#include "Game.h"

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "xaudio2.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace chrono;
using namespace MathFunction;

// ブレンドモード
enum BlendMode {
	//!< ブレンドなし
	kBlendModeNone,
	//!< 通常αブレンド。Src * SrcA + Dest * (1 - SrcA)
	kBlendModeNormal,
	//!< 加算。Src * SrcA + Dest * 1
	kBlendModeAdd,
	//!< 減算。Dest * 1 - Src * SrcA
	kBlendModeSubtract,
	//!< 乗算。Src * 0 + Dest * Src
	kBlendModeMultiply,
	//!< スクリーン。Src * (1 - Dest) + Dest * 1
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

struct CameraForGPU {
	Vector3 worldPosition;
};

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
	// 時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// processId（このexeのId）とクラッシュ（例外）の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	// 設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	// Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	// ほかに関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する

	return EXCEPTION_EXECUTE_HANDLER;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3DResourceLeakChecker leakCheck;
	ComPtr<IDXGIFactory7> dcgiFactory;

	// 誰も補足しなかった場合に(Unhandled)、補足する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	// log出力用のフォルダ「logs」を作成
	filesystem::create_directory("logs");

	// ここからファイルを作成しofstreamを取得する
	// 現在時刻を取得
	system_clock::time_point now = system_clock::now();
	// 削って秒にする
	time_point<system_clock, seconds>
		nowSeconds = time_point_cast<seconds>(now);
	// 日本時間に変換
	zoned_time localTime{ current_zone(), nowSeconds };
	// formatを使って年月日_時分秒の文字列に変換
	string dateString = format("{:%Y%m%d_%H%M%S}", localTime);
	// 時刻を使ってファイル名を決定
	string logFilePath = string("logs/") + dateString + ".log";
	// ファイルを作って書き込み準備
	ofstream logStream(logFilePath);

	Game game;
	game.Initialize();

	
	// ウィンドウの×ボタンが押されるまでループ
	while (true) {

		game.Update();

		if(game.IsEndRequst()) {
			break;
		}

		game.Draw();

	}

	game.Finalize();

//	CloseHandle(dxBase->GetFenceEvent());
//
//	// WindowsAPIの終了処理
//	winApp->Finalize();
//
//	// WindowsAPI解放
//	delete winApp;
//	winApp = nullptr;
//
//	// キー入力処理解放
//	delete input;
//
//	//// スプライト共通部の解放
//	delete spriteCommon;
//
//	// フィールドの解放
////	delete field;
//
//	// playerの解放
//	delete player;
//
//	// カメラの解放
//	delete camera;
//
//	delete gameScene;
//
//	// パーティクルエミッターの解放
//	delete particleEmitter;
//
//	// パーティクルマネージャの終了
//	particleManager->Finalize();
//
//	// 3dオブジェクト共通部の解放
//	delete object3dCommon;
//
//	// テクスチャマネージャの終了
//	textureManager->Finalize();
//
//	// 3Dモデルマネージャの終了
//	modelManager->Finalize();
//
//	// SRVマネージャの解放
//	delete srvManager;
//
//	// DirectX解放
//	delete dxBase;
//
//	// XAudio2解放
//	xAudio2.Reset();
//	// 音声データ解放
//	SoundUnload(&soundData1);

	return 0;
}