/**
 * @file   ShaderManager.cpp
 *
 * @brief  シェーダーマネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ShaderManager.h"
#include <fstream>
#include <assert.h>


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param device	デバイスのポインタ
 */
ShaderManager::ShaderManager(ID3D11Device1* device)
	: m_pDevice{ device }
{

}


/**
 * @brief デストラクタ
 */
ShaderManager::~ShaderManager()
{
	m_vertexShaders.clear();
	m_pixelShaders.clear();
	m_geometryShaders.clear();
}


/**
 * @brief 頂点シェーダーの作成
 *
 * @param name				配列に登録するキー
 * @param filePath			シェーダーファイルへのパス
 * @param inputLayoutDesc	インプットレイアウト
 *
 * @retval true  作成に成功
 * @retval false 作成に失敗
 */
bool ShaderManager::CreateVS(
	const std::string& name, const std::string& filePath, 
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc)
{
	// 名前が重複していないか確認
	if (m_vertexShaders.find(name) != m_vertexShaders.end())
	{
		return false;
	}

	// ファイルを読み込む
	std::vector<uint8_t> bytecode;
	if (!LoadFile(StringToWchar(filePath).c_str(), bytecode)) return false;

	VertexShaderEntry entry{};

	// シェーダーを作成
	auto hr = m_pDevice->CreateVertexShader(bytecode.data(), bytecode.size(), nullptr, entry.vs.GetAddressOf());
	if (FAILED(hr)) return false;

	// バイトコードを保持
	entry.bytecode = bytecode;

	// インプットレイアウトを作成
	if (!inputLayoutDesc.empty())
	{
		hr = m_pDevice->CreateInputLayout(
			inputLayoutDesc.data(),
			static_cast<UINT>(inputLayoutDesc.size()),
			entry.bytecode.data(), entry.bytecode.size(),
			entry.inputLayout.GetAddressOf()
		);
		if (FAILED(hr)) return false;
	}

	// マップに登録
	m_vertexShaders.emplace(name, std::move(entry));

	return true;
}


/**
 * @brief ピクセルシェーダーの作成
 *
 * @param name		配列に登録するキー
 * @param filePath	シェーダーファイルへのパス
 *
 * @retval true  作成に成功
 * @retval false 作成に失敗
 */
bool ShaderManager::CreatePS(const std::string& name, const std::string& filePath)
{
	// 名前が重複していないか確認
	if (m_pixelShaders.find(name) != m_pixelShaders.end())
	{
		return false;
	}

	// ファイルを読み込む
	std::vector<uint8_t> bytecode;
	if (!LoadFile(StringToWchar(filePath).c_str(), bytecode)) return false;

	PixelShaderEntry entry{};

	// シェーダーを作成
	auto hr = m_pDevice->CreatePixelShader(bytecode.data(), bytecode.size(), nullptr, entry.ps.GetAddressOf());
	if (FAILED(hr)) return false;

	// マップに登録
	m_pixelShaders.emplace(name, std::move(entry));

	return true;
}


/**
 * @brief ジオメトリシェーダーの作成
 *
 * @param name		配列に登録するキー
 * @param filePath	シェーダーファイルへのパス
 *
 * @retval true  作成に成功
 * @retval false 作成に失敗
 */
bool ShaderManager::CreateGS(const std::string& name, const std::string& filePath)
{
	// 名前が重複していないか確認
	if (m_geometryShaders.find(name) != m_geometryShaders.end())
	{
		return false;
	}

	// ファイルを読み込む
	std::vector<uint8_t> bytecode;
	if (!LoadFile(StringToWchar(filePath).c_str(), bytecode)) return false;

	GeometryShaderEntry entry{};

	// シェーダーを作成
	auto hr = m_pDevice->CreateGeometryShader(bytecode.data(), bytecode.size(), nullptr, entry.gs.GetAddressOf());
	if (FAILED(hr)) return false;

	// マップに登録
	m_geometryShaders.emplace(name, std::move(entry));

	return true;
}


/**
 * @brief 頂点シェーダーの取得
 *
 * @param name	取得したい頂点シェーダーのキー
 *
 * @return	頂点シェーダーの情報のポインタ
 */
ShaderManager::VertexShaderEntry* ShaderManager::GetVS(const std::string& name)
{
	auto it = m_vertexShaders.find(name);

	// 名前が存在しているか確認
	if (it == m_vertexShaders.end())
	{
		return nullptr;
	}

	return &it->second;
}


/**
 * @brief ピクセルシェーダーの取得
 *
 * @param name	取得したいピクセルシェーダーのキー
 *
 * @return	ピクセルシェーダーの情報のポインタ
 */
ShaderManager::PixelShaderEntry* ShaderManager::GetPS(const std::string& name)
{
	auto it = m_pixelShaders.find(name);

	// 名前が存在しているか確認
	if (it == m_pixelShaders.end())
	{
		return nullptr;
	}

	return &it->second;
}


/**
 * @brief ジオメトリシェーダーの取得
 *
 * @param name	取得したいジオメトリシェーダーのキー
 *
 * @return	ジオメトリシェーダーの情報のポインタ
 */
ShaderManager::GeometryShaderEntry* ShaderManager::GetGS(const std::string& name)
{
	auto it = m_geometryShaders.find(name);

	// 名前が存在しているか確認
	if (it == m_geometryShaders.end())
	{
		return nullptr;
	}

	return &it->second;
}


/**
 * @brief ファイルの読み込み
 *
 * @param filePath	読み込むファイルのパス
 * @param out		出力データ
 *
 * @retval true  読み込み成功
 * @retval false 読み込み失敗
 */
bool ShaderManager::LoadFile(const wchar_t* filePath, std::vector<uint8_t>& out)
{
	std::ifstream ifs;

	// ファイルを開く
	ifs.open(filePath, std::ios::in | std::ios::binary);

	// 読み込み失敗時は強制終了
	if (!ifs) return false;

	// ファイル内容の読み込み
	ifs.seekg(0, std::ios::end);
	const auto size = static_cast<size_t>(ifs.tellg());
	ifs.seekg(0, std::ios::beg);
	out.resize(size);
	if (size) ifs.read(reinterpret_cast<char*>(out.data()), size);

	return static_cast<bool>(ifs);
}



/**
 * @brief string型をwchar_t型へ変換
 *
 * @param str	変換したい文字列
 *
 * @return 変換後の文字列
 */
std::wstring ShaderManager::StringToWchar(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}
