/**
 * @file   ResourceManager.cpp
 *
 * @brief  リソースマネージャに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ResourceManager.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include <iostream>
#include <fstream>
#include <string>


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
ResourceManager::ResourceManager(ID3D11Device* pDevice)
	:m_pDevice{ pDevice }
{
}



/**
 * @brief デストラクタ
 */
ResourceManager::~ResourceManager()
{
	m_pDevice = nullptr;

	m_textures.clear();
	m_models.clear();
	m_animations.clear();
}


/**
 * @brief PNGファイルの読み込み
 *
 * @param key		配列に登録するキー
 * @param filename	画像ファイルへのパス
 *
 * @retval true  読み込みに成功
 * @retval false 読み込みに失敗
 */
bool ResourceManager::LoadPNG(const std::string& key, const wchar_t* filename)
{
	HRESULT hr;

	//まだ登録されていないキーか確認
	auto it = m_textures.find(key);
	if (m_textures.find(key) != m_textures.end())
	{
		std::cerr << "そのキーは既に使用されています: " << key << std::endl;
		return false;
	}

	//PNGファイルの読み込み
	hr = DirectX::CreateWICTextureFromFile(
		m_pDevice,
		filename,
		nullptr,
		m_textures[key].ReleaseAndGetAddressOf()
	);
	if (FAILED(hr))
	{
		std::cerr << "ファイルの読み込みに失敗しました" << std::endl;
		return false;
	}

	return true;
}


/**
 * @brief DDSファイルの読み込み
 *
 * @param key		配列に登録するキー
 * @param filename	画像ファイルへのパス
 *
 * @retval true  読み込みに成功
 * @retval false 読み込みに失敗
 */
bool ResourceManager::LoadDDS(const std::string& key, const wchar_t* filename)
{
	HRESULT hr;

	//まだ登録されていないキーか確認
	auto it = m_textures.find(key);
	if (it != m_textures.end())
	{
		std::cerr << "そのキーは既に使用されています: " << key << std::endl;
		return false;
	}

	//DDSファイルの読み込み
	hr = DirectX::CreateDDSTextureFromFile(
		m_pDevice,
		filename,
		nullptr,
		m_textures[key].ReleaseAndGetAddressOf()
	);
	if (FAILED(hr))
	{
		std::cerr << "ファイルの読み込みに失敗しました" << std::endl;
		return false;
	}

	return true;
}


/**
 * @brief テクスチャの取得(PNG)
 * 
 * キーが配列にあったら渡す、無かったらファイルを読み込んでから渡す
 *
 * @param key		取得したいテクスチャのキー
 * @param filename	画像ファイルへのパス
 *
 * @return テクスチャのポインタ
 */
ID3D11ShaderResourceView* ResourceManager::RequestPNG(const std::string& key, const wchar_t* filename)
{
	//まだ登録されていないキーか確認
	auto it = m_textures.find(key);
	if (it == m_textures.end())
	{
		//登録されていなかったら読み込む
		if (!LoadPNG(key, filename)) return nullptr;
	}

	//キーに対応したテクスチャのポインタを返す
	return m_textures[key].Get();
}


/**
 * @brief テクスチャの取得(DDS)
 *
 * キーが配列にあったら渡す、無かったらファイルを読み込んでから渡す
 *
 * @param key		取得したいテクスチャのキー
 * @param filename	画像ファイルへのパス
 *
 * @return テクスチャのポインタ
 */
ID3D11ShaderResourceView* ResourceManager::RequestDDS(const std::string& key, const wchar_t* filename)
{
	//まだ登録されていないキーか確認
	auto it = m_textures.find(key);
	if (it == m_textures.end())
	{
		//登録されていなかったら読み込む
		if (!LoadDDS(key, filename)) return nullptr;
	}

	//キーに対応したテクスチャのポインタを返す
	return m_textures[key].Get();
}


/**
 * @brief テクスチャの取得
 *
 * @param key	取得したいテクスチャのキー
 *
 * @return テクスチャのポインタ
 */
ID3D11ShaderResourceView* ResourceManager::GetTexture(const std::string& key)
{
	//まだ登録されていないキーか確認
	auto it = m_textures.find(key);
	if (it == m_textures.end())
	{
		std::cerr << "指定されたキーが見つかりません: " << key << std::endl;
		return nullptr;
	}

	//キーに対応したテクスチャのポインタを返す
	return m_textures[key].Get();
}


/**
 * @brief SDKMESHファイルの読み込み
 *
 * @param key		配列に登録するキー
 * @param filename	モデルファイルへのパス
 * @param anim		アニメーションの有無
 *
 * @retval true  読み込みに成功
 * @retval false 読み込みに失敗
 */
bool ResourceManager::LoadSDKMESH(const std::string& key, const wchar_t* filename, bool anim)
{
	//まだ登録されていないキーか確認
	auto it = m_models.find(key);
	if (it != m_models.end())
	{
		std::cerr << "そのキーは既に使用されています: " << key << std::endl;
		return false;
	}

	DirectX::EffectFactory fx(m_pDevice);
	fx.SetDirectory(L"Resources\\Models");

	DirectX::ModelLoaderFlags flags = DirectX::ModelLoaderFlags::ModelLoader_Clockwise;
	if (anim) flags = DirectX::ModelLoaderFlags::ModelLoader_IncludeBones;

	m_models[key] = DirectX::Model::CreateFromSDKMESH(m_pDevice, filename, fx, flags);

	return true;
}


/**
 * @brief モデルの取得
 *
 * @param key	取得したいモデルのキー
 *
 * @return モデルのポインタ
 */
DirectX::Model* ResourceManager::GetModel(const std::string& key)
{
	//まだ登録されていないキーか確認
	auto it = m_models.find(key);
	if (it == m_models.end())
	{
		std::cerr << "指定されたキーが見つかりません: " << key << std::endl;
		return nullptr;
	}

	//キーに対応したモデルのポインタを返す
	return m_models[key].get();
}


/**
 * @brief モデルの取得
 *
 * キーが配列にあったら渡す、無かったらファイルを読み込んでから渡す
 *
 * @param key		取得したいモデルのキー
 * @param filename	モデルファイルへのパス
 *
 * @return モデルのポインタ
 */
DirectX::Model* ResourceManager::RequestSDKMESH(const std::string& key, const wchar_t* filename, bool anim)
{
	//まだ登録されていないキーか確認
	auto it = m_models.find(key);
	if (it == m_models.end())
	{
		//登録されていなかったら読み込む
		if (!LoadSDKMESH(key, filename, anim)) return nullptr;
	}

	//キーに対応したモデルのポインタを返す
	return m_models[key].get();
}


/**
 * @brief モデルの取得
 *
 * キーが配列にあったら渡す、無かったらファイルを読み込んでから渡す
 * ファイルパスをstringからwchar_tに変換
 *
 * @param key		取得したいモデルのキー
 * @param filename	モデルファイルへのパス
 *
 * @return モデルのポインタ
 */
DirectX::Model* ResourceManager::RequestSDKMESH(const std::string& key, const std::string& filename, bool anim)
{
	// ファイルパスを変換してから読み込み
	return RequestSDKMESH(key, StringToWchar(filename).c_str(), anim);
}


/**
 * @brief アニメーションファイルの読み込み
 *
 * @param key		配列に登録するキー
 * @param filename	アニメーションファイルへのパス
 *
 * @retval true  読み込みに成功
 * @retval false 読み込みに失敗
 */
bool ResourceManager::LoadAnimation(const std::string& key, const wchar_t* filename)
{
	auto it = m_animations.find(key);
	if (it != m_animations.end())
	{
		std::cerr << "そのキーは既に使用されています: " << key << std::endl;
		return false;
	}
	
	std::unique_ptr<DX::AnimationSDKMESH> anim = std::make_unique<DX::AnimationSDKMESH>();
	anim->Load(filename);
	m_animations[key] = std::move(anim);

	return true;
}


/**
 * @brief アニメーションの取得
 *
 * @param key	取得したいアニメーションのキー
 *
 * @return アニメーションのポインタ
 */
DX::AnimationSDKMESH* ResourceManager::GetAnimation(const std::string& key)
{
	//まだ登録されていないキーか確認
	auto it = m_animations.find(key);
	if (it == m_animations.end())
	{
		std::cerr << "指定されたキーが見つかりません: " << key << std::endl;
		return nullptr;
	}

	//キーに対応したモデルのポインタを返す
	return m_animations[key].get();
}


/**
 * @brief アニメーションの取得
 *
 * キーが配列にあったら渡す、無かったらファイルを読み込んでから渡す
 *
 * @param key		取得したいアニメーションのキー
 * @param filename	アニメーションファイルへのパス
 *
 * @return アニメーションのポインタ
 */
DX::AnimationSDKMESH* ResourceManager::RequestAnimation(const std::string& key, const wchar_t* filename)
{
	//まだ登録されていないキーか確認
	auto it = m_animations.find(key);
	if (it == m_animations.end())
	{
		//登録されていなかったら読み込む
		if (!LoadAnimation(key, filename)) return nullptr;
	}

	//キーに対応したアニメーションのポインタを返す
	return m_animations[key].get();
}


/**
 * @brief アニメーションの取得
 *
 * キーが配列にあったら渡す、無かったらファイルを読み込んでから渡す
 * ファイルパスをstringからwchar_tに変換
 *
 * @param key		取得したいアニメーションのキー
 * @param filename	アニメーションファイルへのパス
 *
 * @return アニメーションのポインタ
 */
DX::AnimationSDKMESH* ResourceManager::RequestAnimation(const std::string& key, const std::string& filename)
{
	// ファイルパスを変換してから読み込み
	return RequestAnimation(key, StringToWchar(filename).c_str());
}


/**
 * @brief string型をwchar_t型へ変換
 *
 * @param str	変換したい文字列
 *
 * @return 変換後の文字列
 */
std::wstring ResourceManager::StringToWchar(std::string str)
{
	return std::wstring(str.begin(), str.end());
}
