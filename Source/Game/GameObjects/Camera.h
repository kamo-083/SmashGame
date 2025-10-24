/**
 * @file   Camera.h
 *
 * @brief  カメラに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include"Source/Debug/DebugFont.h"



// クラスの定義 ===============================================================
/**
 * @brief カメラ
 */
class Camera
{
	// クラス定数の宣言 -------------------------------------------------
private:
	// カメラのデフォルト設定
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_DEFAULT_EYE    = { 0.0f,4.0f,8.0f };	// 位置
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_DEFAULT_TARGET = { 0.0f,0.0f,0.0f };	// 注視点
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_DEFAULT_UP     = { 0.0f,1.0f,0.0f };	// 上方向ベクトル

	// カメラの回転関連
	static constexpr float CAMERA_ROTATE_ANGLE = 45.0f;		// キー1回分の回転角度
	static constexpr float CAMERA_ROTATE_SPEED = 0.025f;	// 回転速度


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

	// 前方向ベクトル
	DirectX::SimpleMath::Vector3 m_forward;

	// 追従対象の座標
	DirectX::SimpleMath::Vector3* m_followTargetPos;

	// 回転中
	bool m_isRotation;

	// 補間関連
	float m_lerpAngle;
	DirectX::SimpleMath::Vector3 m_startAngle;
	DirectX::SimpleMath::Vector3 m_endAngle;

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
	void Initialize(DirectX::SimpleMath::Vector3* followTargetPos);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(DebugFont* debugFont);

	// 回転操作
	void Rotation(DirectX::Keyboard::KeyboardStateTracker* keyboard);

	// 回転の補間
	void SmoothCameraRotation(float elapsedTime);

// 取得/設定
public:
	// ビュー行列を取得
	DirectX::SimpleMath::Matrix GetView() { return m_view; }

	// カメラの位置を取得
	DirectX::SimpleMath::Vector3 GetEye() { return m_eye; }

	// カメラの注視点を取得
	DirectX::SimpleMath::Vector3 GetTarget() { return m_target; }

	// カメラの上方向を取得
	DirectX::SimpleMath::Vector3 GetUp() { return CAMERA_DEFAULT_UP; }

	// 前方向ベクトルを取得
	DirectX::SimpleMath::Vector3 GetForward();

	// 追従対象の座標を設定
	void SetFollowTargetPos(DirectX::SimpleMath::Vector3* pos){ m_followTargetPos = pos; }

// 内部実装
private:
	// 指定座標を中心にカメラ位置を回転
	DirectX::SimpleMath::Vector3 RotateEyeAroundPoint(DirectX::SimpleMath::Vector3 angle, DirectX::SimpleMath::Vector3 point);

	// 角度の正規化
	float NormalizeAngle(float angle);
};
