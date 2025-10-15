/**
 * @file   Camera.cpp
 *
 * @brief  カメラに関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Camera.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Camera::Camera()
	: m_eye{ CAMERA_DEFAULT_EYE }
	, m_target{ CAMERA_DEFAULT_TARGET }
	, m_angle{ DirectX::SimpleMath::Vector3::Zero }
	, m_followTargetPos{ nullptr }
	, m_isRotation{ false }
	, m_lerpAngle{ 0.0f }
{

}



/**
 * @brief デストラクタ
 */
Camera::~Camera()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Camera::Initialize(DirectX::SimpleMath::Vector3* followTargetPos)
{
	m_eye = CAMERA_DEFAULT_EYE;
	m_target = CAMERA_DEFAULT_TARGET;
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_eye, m_target, CAMERA_DEFAULT_UP);

	m_angle = DirectX::SimpleMath::Vector3::Zero;
	m_followTargetPos = followTargetPos;

	m_forward = m_target - m_eye;
	m_forward.y = (*m_followTargetPos).y;
	m_forward.Normalize();

	m_isRotation = false;
	m_lerpAngle = 0.0f;
	m_startAngle = m_endAngle = DirectX::SimpleMath::Vector3::Zero;
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Camera::Update(DirectX::Keyboard::KeyboardStateTracker* keyboard, float elapsedTime)
{
	// 回転の補間
	if (m_isRotation) SmoothCameraRotation(elapsedTime);

	// カメラ位置の更新
	m_eye = *m_followTargetPos + CAMERA_DEFAULT_EYE;

	// 対象に追従して動く
	m_target = *m_followTargetPos + CAMERA_DEFAULT_TARGET;

	// 回転の反映
	m_eye = RotateEyeAroundPoint(m_angle, *m_followTargetPos);

	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_eye, m_target, CAMERA_DEFAULT_UP);
}

void Camera::Draw(Imase::DebugFont* debugFont)
{
	// デバッグ情報の描画
	debugFont->AddString(820, 0, DirectX::Colors::White, L"eye = %f,%f,%f", m_eye.x, m_eye.y, m_eye.z);
	debugFont->AddString(820, 25, DirectX::Colors::White, L"target = %f,%f,%f", m_target.x, m_target.y, m_target.z);
	debugFont->AddString(820, 50, DirectX::Colors::White, L"angle = %f,%f,%f", m_angle.x, m_angle.y, m_angle.z);
	debugFont->AddString(820, 75, DirectX::Colors::White, L"forward = %f,%f,%f", m_forward.x, m_forward.y, m_forward.z);
}

void Camera::Rotation(DirectX::Keyboard::KeyboardStateTracker* keyboard)
{
	// 指定した場所を中心に回転
	// X方向
	if (keyboard->pressed.L)		m_endAngle.x += CAMERA_ROTATE_ANGLE;
	else if (keyboard->pressed.J)	m_endAngle.x -= CAMERA_ROTATE_ANGLE;

	m_startAngle = m_angle;

	// 回転中に設定
	m_isRotation = true;
}

void Camera::SmoothCameraRotation(float elapsedTime)
{
	// カメラの回転(線形補間)
	m_angle = DirectX::SimpleMath::Vector3::Lerp(m_startAngle, m_endAngle, m_lerpAngle);

	// 補間係数の計算
	m_lerpAngle += CAMERA_ROTATE_SPEED * elapsedTime;
	if (m_lerpAngle >= 1.0f)
	{
		m_lerpAngle = 0.0f;
		m_isRotation = false;

		// 角度の調整
		//m_angle.x = NormalizeAngle(m_angle.x);
	}
}

DirectX::SimpleMath::Matrix Camera::GetView()
{
	return m_view;
}

DirectX::SimpleMath::Vector3 Camera::GetEye()
{
	return m_eye;
}

DirectX::SimpleMath::Vector3 Camera::GetTarget()
{
	return m_target;
}

DirectX::SimpleMath::Vector3 Camera::GetUp()
{
	return CAMERA_DEFAULT_UP;
}

DirectX::SimpleMath::Vector3 Camera::GetForward()
{
	m_forward = m_target - m_eye;
	m_forward.y = 0.0f;
	//m_forward.Normalize();

	return m_forward;
}

void Camera::SetFollowTargetPos(DirectX::SimpleMath::Vector3* pos)
{
	m_followTargetPos = pos;
}

DirectX::SimpleMath::Vector3 Camera::RotateEyeAroundPoint(DirectX::SimpleMath::Vector3 angle, DirectX::SimpleMath::Vector3 point)
{
	DirectX::SimpleMath::Vector3 rotatedEye = { 0.0f,0.0f,0.0f };
	float radian_x = DirectX::XMConvertToRadians(angle.y);	//m_angleを操作するときに分かりやすいようにxとyを逆にして計算
	float radian_y = DirectX::XMConvertToRadians(angle.x);
	float radian_z = DirectX::XMConvertToRadians(angle.z);

	// カメラ位置から中心点へのベクトル
	DirectX::SimpleMath::Vector3 vec = m_eye - point;

	// 回転行列を作成
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(radian_y, radian_x, radian_z);

	// ベクトルを回転
	rotatedEye = DirectX::SimpleMath::Vector3::Transform(vec, rot);

	// 中心座標を反映
	rotatedEye += point;

	return rotatedEye;
}

float Camera::NormalizeAngle(float angle)
{
	return std::min(std::max(angle, 0.0f), 360.0f);
}
