/**
 * @file   ShaderManager.cpp
 *
 * @brief  ShaderManagerに関するソースファイル
 *
 * @author 清水まこと
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
 * @param[in] なし
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

bool ShaderManager::CreateVS(const std::string& name, const wchar_t* filePath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc)
{
	// 名前が重複していないか確認
	if (m_vertexShaders.find(name) != m_vertexShaders.end())
	{
		return false;
	}

	// ファイルを読み込む
	std::vector<uint8_t> bytecode;
	if (!LoadFile(filePath, bytecode)) return false;

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

bool ShaderManager::CreatePS(const std::string& name, const wchar_t* filePath)
{
	// 名前が重複していないか確認
	if (m_pixelShaders.find(name) != m_pixelShaders.end())
	{
		return false;
	}

	// ファイルを読み込む
	std::vector<uint8_t> bytecode;
	if (!LoadFile(filePath, bytecode)) return false;

	PixelShaderEntry entry{};

	// シェーダーを作成
	auto hr = m_pDevice->CreatePixelShader(bytecode.data(), bytecode.size(), nullptr, entry.ps.GetAddressOf());
	if (FAILED(hr)) return false;

	// マップに登録
	m_pixelShaders.emplace(name, std::move(entry));

	return true;
}

bool ShaderManager::CreateGS(const std::string& name, const wchar_t* filePath)
{
	// 名前が重複していないか確認
	if (m_geometryShaders.find(name) != m_geometryShaders.end())
	{
		return false;
	}

	// ファイルを読み込む
	std::vector<uint8_t> bytecode;
	if (!LoadFile(filePath, bytecode)) return false;

	GeometryShaderEntry entry{};

	// シェーダーを作成
	auto hr = m_pDevice->CreateGeometryShader(bytecode.data(), bytecode.size(), nullptr, entry.gs.GetAddressOf());
	if (FAILED(hr)) return false;

	// マップに登録
	m_geometryShaders.emplace(name, std::move(entry));

	return true;
}

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
