/**
 * @file   AudioManager.h
 *
 * @brief  音声の読込/操作に関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include <iostream>
#include <fstream>

//XAudio2関連
#include <xaudio2.h>
#include <mmreg.h> 
#pragma comment(lib,"xaudio2.lib")

//MediaFoundation関連
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <unordered_map>


// クラスの定義 ===============================================================
/**
 * @brief 音声の読込/操作
 */
class AudioManager
{
	// クラス定数の宣言 -------------------------------------------------
private:
	struct WAVHeader {
		char riff[4];            //"RIFF"文字列
		uint32_t file_size;      //ファイルサイズ - 8
		char wave[4];            //"WAVE"文字列

		char fmt[4];             //"fmt"文字列(スペースを含む)
		uint32_t fmt_size;       //フォーマットサイズ(16ならPCM)
		uint16_t audio_format;   //音声フォーマット(1 = PCM)
		uint16_t num_channels;   //チャンネル数(1 = モノラル, 2 = ステレオ)
		uint32_t sample_rate;    //サンプリングレート(44100, 48000 など)
		uint32_t byte_rate;      //データ転送レート(sample_rate * num_channels * bits_per_sample / 8)
		uint16_t block_align;    //ブロックサイズ(num_channels * bits_per_sample / 8)
		uint16_t bits_per_sample;//1サンプルあたりのビット数(16, 24, 32 など)

		char data[4];            //"data"文字列
		uint32_t data_size;      //実際の音声データのサイズ
	};

	struct AudioData {
		std::vector<BYTE> data;							//音声データ
		WAVEFORMATEX format;							//WAVのフォーマット情報
		XAUDIO2_BUFFER buffer;							//XAudio2で再生するためのバッファ
			
			
			
			
			;							//XAudio2で再生するためのバッファ
		IXAudio2SourceVoice* pSourceVoice = nullptr;	//再生用のソースボイス
	};

	// データメンバの宣言 -----------------------------------------------
private:
	IXAudio2* m_pXAudio2;						//XAudio2エンジン
	IXAudio2MasteringVoice* m_pMasteringVoice;	//マスターボイス

	std::unordered_map<std::string, AudioData> m_sounds;

	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	AudioManager();

	// デストラクタ
	~AudioManager();


// 操作
public:
	//WAVの読み込み
	bool LoadWAV(const std::string& key, const std::string& filename);

	//MP3の読み込み
	bool LoadMP3(const std::string& key, const std::string& filename);

	//音声の再生
	void Play(const std::string& key, bool loop);

	//音声の停止
	void Stop(const std::string& key);

	//音声の状態を取得
	bool IsPlaying(const std::string& key);

// 内部実装
private:


};
