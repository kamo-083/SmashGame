/**
 * @file   Event.h
 *
 * @brief  イベントに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================


// クラスの定義 ===============================================================
/**
 * @brief Event
 */
class Event
{
public:

	// イベントID
	enum class EventID
	{
		// プレイヤー
		PLAYER_MOVE_UP,		// 移動・上
		PLAYER_MOVE_DOWN,	// 移動・下
		PLAYER_MOVE_LEFT,	// 移動・左
		PLAYER_MOVE_RIGHT,	// 移動・右
		PLAYER_ATTACK,		// 攻撃
		PLAYER_KNOCKBACK,	// 吹っ飛ぶ

		// 敵
		ENEMY_ATTACK,		// 攻撃
		ENEMY_KNOCKBACK,	// 吹っ飛ぶ

		// 攻撃
		ATTACK_CHANGE_LEFT,		// 変更・左
		ATTACK_CHANGE_RIGHT,	// 変更・右

		// カメラ
		CAMERA_ROTATE_LEFT,		// 回転・左
		CAMERA_ROTATE_RIGHT,	// 回転・右

		// 攻撃変更・カメラ回転の切り替え
		SWITCH_ATTACK_CAMERA,

		// ポーズの開閉
		PAUSE_OPEN_CLOSE,
	};
};
