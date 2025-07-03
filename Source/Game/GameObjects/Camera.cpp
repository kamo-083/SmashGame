/**
 * @file   Camera.cpp
 *
 * @brief  カメラに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Camera.h"


using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Camera::Camera()
	:m_eye{ CAMERA_DEFAULT_EYE }
	,m_target{ CAMERA_DEFAULT_TARGET }
	,m_angle{ SimpleMath::Vector3::Zero }
	,m_followTargetPos{ SimpleMath::Vector3::Zero }
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
void Camera::Initialize()
{
	m_eye = CAMERA_DEFAULT_EYE;
	m_target = CAMERA_DEFAULT_TARGET;
	m_view = SimpleMath::Matrix::CreateLookAt(m_eye, m_target, CAMERA_DEFAULT_UP);

	m_angle = SimpleMath::Vector3::Zero;
	m_followTargetPos = SimpleMath::Vector3::Zero;

	m_forward = m_target - m_eye;
	m_forward.y = m_followTargetPos.y;
	m_forward.Normalize();
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Camera::Update(Keyboard::KeyboardStateTracker* keyboard, float elapsedTime)
{
	// カメラ位置の更新
	m_eye = m_followTargetPos + CAMERA_DEFAULT_EYE;

	// 指定した場所を中心に回転
	// X方向
	if (keyboard->pressed.D)			m_angle.x += CAMERA_ROTATE_ANGLE;
	else if (keyboard->pressed.A)		m_angle.x -= CAMERA_ROTATE_ANGLE;
	// Y方向	一定の範囲で往復してるような動きになっている
	if (keyboard->pressed.W)			m_angle.y += CAMERA_ROTATE_ANGLE;
	else if (keyboard->pressed.S)		m_angle.y -= CAMERA_ROTATE_ANGLE;
	// Z方向
	if (keyboard->pressed.Q)			m_angle.z += CAMERA_ROTATE_ANGLE;
	else if (keyboard->pressed.E)		m_angle.z -= CAMERA_ROTATE_ANGLE;

	// 角度の調整
	m_angle.x = NormalizeAngle(m_angle.x);

	// 回転の反映
	m_eye = RotateEyeAroundPoint(m_angle, m_followTargetPos);

	// 対象に追従して動く
	m_target = m_followTargetPos + CAMERA_DEFAULT_TARGET;

	// リセット
	if (keyboard->pressed.Space)
	{
		m_eye = CAMERA_DEFAULT_EYE;
		m_target = CAMERA_DEFAULT_TARGET;
		m_angle = SimpleMath::Vector3::Zero;
	}

	m_view = SimpleMath::Matrix::CreateLookAt(m_eye, m_target, CAMERA_DEFAULT_UP);
}

void Camera::Draw(Imase::DebugFont* debugFont)
{
	debugFont->AddString(0, 0, Colors::White, L"eye = %f,%f,%f", m_eye.x, m_eye.y, m_eye.z);
	debugFont->AddString(0, 25, Colors::White, L"target = %f,%f,%f", m_target.x, m_target.y, m_target.z);
	debugFont->AddString(0, 50, Colors::White, L"angle = %f,%f,%f", m_angle.x, m_angle.y, m_angle.z);

	debugFont->AddString(0, 75, Colors::White, L"forward = %f,%f,%f", m_forward.x, m_forward.y, m_forward.z);
}

SimpleMath::Matrix Camera::GetView()
{
	return m_view;
}

SimpleMath::Vector3 Camera::GetEye()
{
	return m_eye;
}

SimpleMath::Vector3 Camera::GetTarget()
{
	return m_target;
}

SimpleMath::Vector3 Camera::GetUp()
{
	return CAMERA_DEFAULT_UP;
}

SimpleMath::Vector3 Camera::GetForward()
{
	m_forward = m_target - m_eye;
	m_forward.y = 0.0f;
	//m_forward.Normalize();

	return m_forward;
}

void Camera::SetFollowTargetPos(SimpleMath::Vector3 pos)
{
	m_followTargetPos = pos;
}

SimpleMath::Vector3 Camera::RotateEyeAroundPoint(SimpleMath::Vector3 angle, SimpleMath::Vector3 point)
{	
	SimpleMath::Vector3 rotatedEye = { 0.0f,0.0f,0.0f };
	float radian_x = XMConvertToRadians(angle.y);	//m_angleを操作するときに分かりやすいようにxとyを逆にして計算
	float radian_y = XMConvertToRadians(angle.x);
	float radian_z = XMConvertToRadians(angle.z);

	// カメラ位置から中心点へのベクトル
	SimpleMath::Vector3 vec = m_eye - point;

	// 回転行列を作成
	SimpleMath::Matrix rot = SimpleMath::Matrix::CreateFromYawPitchRoll(radian_y, radian_x, radian_z);
	
	// ベクトルを回転
	rotatedEye = SimpleMath::Vector3::Transform(vec, rot);

	// 中心座標を反映
	rotatedEye += point;

	return rotatedEye;
}

float Camera::NormalizeAngle(float angle)
{
	return fmod(angle + 360.0f, 360.0f);
}
