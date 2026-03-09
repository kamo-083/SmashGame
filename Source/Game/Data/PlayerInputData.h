// 多重インクルードの防止 =====================================================
#pragma once

// 移動方向
struct InputDirection
{
	bool forward;	// 前
	bool backward;	// 後ろ
	bool left;		// 左
	bool right;		// 右

	// 全ての状態を初期化
	void Reset()
	{
		forward = false;
		backward = false;
		left = false;
		right = false;
	}
};
