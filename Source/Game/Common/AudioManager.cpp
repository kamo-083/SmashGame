/**
 * @file   AudioManager.cpp
 *
 * @brief  音声の読込/操作に関するソースファイル
 *
 * @author 清水まこと
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "AudioManager.h"




// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
AudioManager::AudioManager()
	:m_pXAudio2{nullptr}
	,m_pMasteringVoice{nullptr}
{
	HRESULT result;

	//COMの初期化
	result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(result))
	{
		std::cerr << "COMの初期化に失敗しました。" << std::endl;
		return;
	}

	//XAudio2の初期化
	result = XAudio2Create(&m_pXAudio2, 0);
	if (FAILED(result))
	{
		std::cerr << "XAudio2の初期化に失敗しました。" << std::endl;
		return;
	}

	//MediaFoundationの初期化
	result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	if (FAILED(result))
	{
		std::cerr << "MediaFoundationの初期化に失敗しました。" << std::endl;
		return;
	}

	//マスターボイスの初期化
	result = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(result))
	{
		std::cerr << "マスターボイスの初期化に失敗しました。" << std::endl;
		return;
	}
}



/**
 * @brief デストラクタ
 */
AudioManager::~AudioManager()
{
	for (auto& pair : m_sounds)
	{
		auto& audio = pair.second;
		if (audio.pSourceVoice) audio.pSourceVoice->DestroyVoice();
	}

	if (m_pMasteringVoice) m_pMasteringVoice->DestroyVoice();
	if (m_pXAudio2) m_pXAudio2->Release();
	MFShutdown();
	CoUninitialize();
}

bool AudioManager::LoadWAV(const std::string& key, const std::string& filename)
{
	//ファイルを開く
	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		std::cerr << "WAVファイルを開けません: " << filename << std::endl;
		return false;
	}

	//ヘッダー情報を読み込む
	WAVHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
	if (header.audio_format != 1) 
	{
		std::cerr << "PCMフォーマットではありません。" << std::endl;
		return false;
	}

	//音声データを読み込む
	AudioData audio;
	audio.data.resize(header.data_size);
	file.read(reinterpret_cast<char*>(audio.data.data()), header.data_size);
	file.close();

	if (header.bits_per_sample == 8)
	{
		//8ビットPCMを16ビットPCMに変換
		std::vector<BYTE> convertedData(header.data_size * 2);
		for (size_t i = 0; i < header.data_size; ++i) 
		{
			//8ビットPCMのデータは 0〜255 なので、16ビットに拡張
			convertedData[i * 2] = (header.data[i] - 128) * 256;		  //下位バイト
			convertedData[i * 2 + 1] = (header.data[i] - 128) * 256 >> 8; //上位バイト
		}

		//16ビットPCMとしてWAVEFORMATEXを更新
		header.bits_per_sample = 16;
		header.byte_rate = header.sample_rate * header.num_channels * (header.bits_per_sample / 8);
		header.block_align = header.num_channels * (header.bits_per_sample / 8);
		header.data_size *= 2;
	}

	if (audio.data.empty())
	{
		std::cout << "Error: WAV data is empty!" << std::endl;
	}

	audio.format.wFormatTag = WAVE_FORMAT_PCM;
	audio.format.nChannels = header.num_channels;
	audio.format.nSamplesPerSec = header.sample_rate;
	audio.format.wBitsPerSample = 16;
	audio.format.nBlockAlign = (audio.format.nChannels * audio.format.wBitsPerSample) / 8;
	audio.format.nAvgBytesPerSec = audio.format.nSamplesPerSec * audio.format.nBlockAlign;
	audio.format.cbSize = 0;
	
	//ソースボイスの作成
	HRESULT hr = m_pXAudio2->CreateSourceVoice(&audio.pSourceVoice, &audio.format);
	if (FAILED(hr))
	{
		std::cerr << "ソースボイスの作成に失敗しました。" << std::endl;
		return false;
	}

	//バッファの設定
	audio.buffer.AudioBytes = header.data_size;
	audio.buffer.pAudioData = audio.data.data();
	audio.buffer.Flags = XAUDIO2_END_OF_STREAM;

	//データを保存
	m_sounds[key] = std::move(audio);

	//読込が上手くいっていないため、関数の最後まで通っても再生ができません
	return true;
}

bool AudioManager::LoadMP3(const std::string& key, const std::string& filename)
{
	HRESULT hr;

	//まだ登録されていないキーか確認
	auto it = m_sounds.find(key);
	if (it != m_sounds.end())
	{
		std::cerr << "そのキーは既に使用されています: " << key << std::endl;
		return false;
	}

	//ソースリーダーの作成
	//MP3ファイルの読み込み
	IMFSourceReader* pSourceReader = nullptr;
	hr = MFCreateSourceReaderFromURL(std::wstring(filename.begin(), filename.end()).c_str(), nullptr, &pSourceReader);
	if (FAILED(hr)) 
	{
		std::cerr << "MP3ファイルの読み込みに失敗しました。" << std::endl;
		return false;
	}

	//出力フォーマットをPCM(WAV)に設定
	//MP3をPCM(WAV)に変換
	IMFMediaType* pMediaType = nullptr;
	hr = MFCreateMediaType(&pMediaType);
	if (FAILED(hr))
	{
		std::cerr << "メディアタイプの作成に失敗しました。" << std::endl;
		pSourceReader->Release();
		MFShutdown();
		return false;
	}
	pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	hr = pSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMediaType);
	pMediaType->Release();

	if (FAILED(hr)) 
	{
		std::cerr << "PCM形式への変換に失敗しました。" << std::endl;
		pSourceReader->Release();
		return false;
	}

	//フォーマット情報の取得
	IMFMediaType* pOutputType = nullptr;
	hr = pSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputType);
	if (FAILED(hr))
	{
		std::cerr << "フォーマット情報の取得に失敗しました。" << std::endl;
		pSourceReader->Release();
		MFShutdown();
		return false;
	}

	//WAVフォーマットのデータを作成
	WAVEFORMATEX* pWaveFormat = nullptr;
	UINT32 waveFormatSize = 0;
	MFCreateWaveFormatExFromMFMediaType(pOutputType, &pWaveFormat, &waveFormatSize);
	pOutputType->Release();

	//PCM音声データを読み込む
	std::vector<BYTE> audioData;
	while (true) {
		IMFSample* pSample = nullptr;
		DWORD flags = 0;

		hr = pSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &pSample);
		if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM))
		{
			break;  //終了
		}

		IMFMediaBuffer* pBuffer = nullptr;
		pSample->ConvertToContiguousBuffer(&pBuffer);

		BYTE* pAudioBuffer = nullptr;
		DWORD bufferLength = 0;
		pBuffer->Lock(&pAudioBuffer, nullptr, &bufferLength);

		audioData.insert(audioData.end(), pAudioBuffer, pAudioBuffer + bufferLength);

		pBuffer->Unlock();
		pBuffer->Release();
		pSample->Release();
	}

	//バッファを作成
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = static_cast<UINT32>(audioData.size());
	buffer.pAudioData = audioData.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	//ソースボイスを作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = m_pXAudio2->CreateSourceVoice(&pSourceVoice, pWaveFormat);
	if (FAILED(hr))
	{
		std::cerr << "ソースボイスの作成に失敗しました。" << std::endl;
		pSourceReader->Release();
		MFShutdown();
		return false;
	}

	//データを保存
	AudioData audio;
	audio.data = std::move(audioData);
	audio.format = *pWaveFormat;
	audio.buffer = buffer;
	audio.pSourceVoice = pSourceVoice;
	m_sounds[key] = std::move(audio);

	//メモリ解放
	CoTaskMemFree(pWaveFormat);
	pSourceReader->Release();
	return true;
}

void AudioManager::Play(const std::string& key, bool loop)
{
	HRESULT hr;

	auto it = m_sounds.find(key);
	if (it == m_sounds.end())
	{
		std::cerr << "指定されたキーの音声が見つかりません: " << key << std::endl;
		return;
	}

	AudioData& audio = it->second;

	if (loop) it->second.buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	else	  it->second.buffer.LoopCount = 0;

	if (IsPlaying(key))
	{
		Stop(key);
	}

	hr = audio.pSourceVoice->SubmitSourceBuffer(&audio.buffer);
	if (FAILED(hr))	return;

	hr = audio.pSourceVoice->Start(0);
	if (FAILED(hr))	return;

	std::cout << "再生中: " << key << std::endl;
}

void AudioManager::Stop(const std::string& key)
{
	HRESULT hr;

	auto it = m_sounds.find(key);
	if (it == m_sounds.end())
	{
		std::cerr << "指定されたキーの音声が見つかりません: " << key << std::endl;
		return;
	}

	AudioData& audio = it->second;
	it->second.buffer.LoopCount = 0;

	hr = audio.pSourceVoice->Stop(0);
	if (FAILED(hr))	return;

	audio.pSourceVoice->FlushSourceBuffers();

	std::cout << "停止: " << key << std::endl;
}

bool AudioManager::IsPlaying(const std::string& key)
{
	auto it = m_sounds.find(key);
	if (it == m_sounds.end())
	{
		std::cerr << "指定されたキーの音声が見つかりません: " << key << std::endl;
		return false;
	}

	XAUDIO2_VOICE_STATE state;
	it->second.pSourceVoice->GetState(&state);

	return state.BuffersQueued > 0;
}
