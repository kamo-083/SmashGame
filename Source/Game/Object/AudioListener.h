/**
 * @file	AudioListener.h
 *
 * @brief  音声再生用のメッセージ受信に関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

 // ヘッダファイルの読み込み ===================================================
#include "Source/Game/Interface/IMessageListener.h"
#include "Source/Game/Common/AudioManager.h"

// クラスの定義 ===============================================================
/**
 * @brief 音声再生用のメッセージ受信
 */
class AudioListener : public IMessageListener
{
	// クラス定数の宣言 -------------------------------------------------
public:

	// データメンバの宣言 -----------------------------------------------
private:
	// オーディオマネージャーのポインタ
	AudioManager* m_pAudio;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	AudioListener(AudioManager* pAudio);

	// デストラクタ
	virtual ~AudioListener();

	// 操作
public:
	// 初期化処理
	virtual void Initialize();

	// 終了処理
	virtual void Finalize();

	// メッセージの受信
	void OnMessageAccepted(Message::MessageID messageID) override;

	// 取得/設定
public:

// 内部実装
private:

};