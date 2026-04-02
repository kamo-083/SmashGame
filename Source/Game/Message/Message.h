/**
 * @file   Message.h
 *
 * @brief  メッセージに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once


// ヘッダファイルの読み込み ===================================================


// クラスの定義 ===============================================================
/**
 * @brief Message
 */
class Message
{
public:

	// イベントID
	enum class MessageID
	{
		// プレイヤー
		PLAYER_IDLE,			// 待機
		PLAYER_MOVE_FORWARD,	// 移動・上
		PLAYER_MOVE_BACKWARD,	// 移動・下
		PLAYER_MOVE_LEFT,		// 移動・左
		PLAYER_MOVE_RIGHT,		// 移動・右
		PLAYER_ATTACK,			// 攻撃
		PLAYER_KNOCKBACK,		// 吹っ飛ぶ

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

		// SEの再生
		SE_BOUNCE,			// 跳ね返る
		SE_ATTACK,			// 攻撃
		SE_CHANGE_ATTACK,	// 攻撃の種類を変更
		SE_CURSOR,			// カーソル移動
		SE_GIMMIC_SOLVE,	// ギミックを解いた
		SE_KEY_SPAWN,		// カギの出現
		SE_GOAL_OPEN,		// ゴールの開放
		SE_STAGE_CLEAR,		// ステージクリア
	};
};
