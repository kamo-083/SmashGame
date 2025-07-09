/**
 * @file   Camera.h
 *
 * @brief  カメラに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"ImaseLib/DebugFont.h"



// クラスの定義 ===============================================================
/**
 * @brief カメラ
 */
class Camera
{
	// クラス定数の宣言 -------------------------------------------------
private:
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_DEFAULT_EYE    = { 0.0f,6.0f,10.0f };
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_DEFAULT_TARGET = { 0.0f,0.0f,0.0f };
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_DEFAULT_UP     = { 0.0f,1.0f,0.0f };
	static constexpr float CAMERA_MOVE_SPEED = 5.0f;
	static constexpr float CAMERA_ROTATE_ANGLE = 45.0f;

	// データメンバの宣言 -----------------------------------------------
private:
	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// 位置
	DirectX::SimpleMath::Vector3 m_eye;

	// 注視点
	DirectX::SimpleMath::Vector3 m_target;

	// 角度
	DirectX::SimpleMath::Vector3 m_angle;

	DirectX::SimpleMath::Vector3 m_forward;

	// 追従対象の座標
	DirectX::SimpleMath::Vector3 m_followTargetPos;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	Camera();

	// デストラクタ
	~Camera();


// 操作
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update(DirectX::Keyboard::KeyboardStateTracker* keyboard, float elapsedTime);

	// 描画処理
	void Draw(Imase::DebugFont* debugFont);

	// 回転操作
	void Rotation(DirectX::Keyboard::KeyboardStateTracker* keyboard);

// 取得/設定
public:
	// ビュー行列を取得
	DirectX::SimpleMath::Matrix GetView();

	// カメラの位置を取得
	DirectX::SimpleMath::Vector3 GetEye();

	// カメラの注視点を取得
	DirectX::SimpleMath::Vector3 GetTarget();

	// カメラの上方向を取得
	DirectX::SimpleMath::Vector3 GetUp();

	// 前方向ベクトルを取得
	DirectX::SimpleMath::Vector3 GetForward();

	// 追従対象の座標を設定
	void SetFollowTargetPos(DirectX::SimpleMath::Vector3 pos);

// 内部実装
private:
	DirectX::SimpleMath::Vector3 RotateEyeAroundPoint(DirectX::SimpleMath::Vector3 angle, DirectX::SimpleMath::Vector3 point);

	float NormalizeAngle(float angle);
};
