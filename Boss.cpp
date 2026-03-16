#include "Boss.h"
#include "Object3dCommon.h"
#include "ModelManager.h" // ★これを超重要追加！
#include "Camera.h"

// 引数から ModelCommon を消してスッキリさせました
void Boss::Initialize(Object3dCommon* object3dCommon, Camera* camera) {

    // ==========================================
    // 1. 胴体（Body）の準備
    // ==========================================
    // ① マネージャーに名前を教えて読み込んでもらうだけ！
    ModelManager::GetInstance()->LoadModel("alphaBoss.obj");

    objectBody_ = new Object3d();
    objectBody_->Initialize(object3dCommon);
    // ② オブジェクトに文字列を渡すだけでセット完了！
    objectBody_->SetModel("alphaBoss.obj");


    // ==========================================
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
     // ① サイズを小さくする（0.1倍など、ちょうどいいサイズを探します）
    Vector3 bossScale = { 0.1f, 0.1f, 0.1f }; // ★ここを 0.5f や 0.05f などに変えて調整
    objectBody_->SetScale(bossScale);
    objectLeftArm_->SetScale(bossScale);
    objectRightArm_->SetScale(bossScale);

    // ② カメラから少し離れた位置（奥）に置く
    // 今カメラが Z: -10.0f にいるので、ボスを Z: 10.0f くらいに置くと全体が見えやすいです
    Vector3 bossPos = { 0.0f, 0.0f, 10.0f };
    objectBody_->SetTranslate(bossPos);

    // 腕の位置も胴体に合わせて調整（※ボスのモデルの作り方によります）
    // もし腕が胴体と同じ原点で作られているモデルなら、腕も同じ座標でOKです！
    objectLeftArm_->SetTranslate(bossPos);
    objectRightArm_->SetTranslate(bossPos);
    // ==========================================
    // 5. 向き（回転）の設定
    // ==============================,============
    // { X軸の回転, Y軸の回転, Z軸の回転 } です。
    // コマみたいに横を向かせたい（旋回させたい）場合は、真ん中の「Y」の値をいじります。

    // 例：180度回して反対を向かせる
    Vector3 bossRotate = { 0.0f, 3.14f, 0.0f }; 
    objectBody_->SetRotate(bossRotate);
    objectLeftArm_->SetRotate(bossRotate);
    objectRightArm_->SetRotate(bossRotate);

    float armOffset = 0.5f;

    // 左腕はマイナス方向、右腕はプラス方向にズラす
  // もし前後（胸と背中）に腕がいってしまった場合の書き方
    Vector3 leftArmPos = { bossPos.x, bossPos.y, bossPos.z - armOffset };
    Vector3 rightArmPos = { bossPos.x, bossPos.y, bossPos.z + armOffset };

    objectLeftArm_->SetTranslate(leftArmPos);
    objectRightArm_->SetTranslate(rightArmPos);

    objectBody_->SetCamera(camera);
    objectLeftArm_->SetCamera(camera);
    objectRightArm_->SetCamera(camera);
}

void Boss::Update() {
    // 胴体と腕の行列更新
    objectBody_->Update();
    objectLeftArm_->Update();
    objectRightArm_->Update();
}

void Boss::Draw() {
    // 描画
    if (objectBody_) { objectBody_->Draw(); }
    if (objectLeftArm_) { objectLeftArm_->Draw(); }
    if (objectRightArm_) { objectRightArm_->Draw(); }
}


//
//次はボスの攻撃（ロケットパンチ）
//