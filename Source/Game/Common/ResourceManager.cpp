/**
 * @file   ResourceManager.cpp
 *
 * @brief  リソースマネージャに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
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
 * @param[in] なし
 */
ResourceManager::ResourceManager(ID3D11Device* pDevice)
	:m_pDevice{ pDevice }
	,m_textures{}
	,m_models{}
{

}



/**
 * @brief デストラクタ
 */
ResourceManager::~ResourceManager()
{
	//念のため初期化
	if(!m_textures.empty())	m_textures.clear();
	if(!m_models.empty())	m_models.clear();
}

bool ResourceManager::LoadPNG(const std::string& key, const wchar_t* filename)
{
	HRESULT hr;

	//まだ登録されていないキーか確認
	auto it = m_textures.find(key);
	if (it != m_textures.end())
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

ID3D11ShaderResourceView* ResourceManager::RequestTexture(const std::string& key, const wchar_t* filename)
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

bool ResourceManager::LoadSDKMESH(const std::string& key, const wchar_t* filename)
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

	m_models[key] = DirectX::Model::CreateFromSDKMESH(m_pDevice, filename, fx);

	return true;
}

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

DirectX::Model* ResourceManager::RequestSDKMESH(const std::string& key, const wchar_t* filename)
{
	//まだ登録されていないキーか確認
	auto it = m_models.find(key);
	if (it == m_models.end())
	{
		//登録されていなかったら読み込む
		if (!LoadSDKMESH(key, filename)) return nullptr;
	}

	//キーに対応したモデルのポインタを返す
	return m_models[key].get();
}
