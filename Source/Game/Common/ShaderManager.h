/**
 * @file   ShaderManager.h
 *
 * @brief  ShaderManagerに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include<unordered_map>
#include<string>
#include<vector>


// クラスの定義 ===============================================================
/**
 * @brief ShaderManager
 */
class ShaderManager
{
	// クラス定数の宣言 -------------------------------------------------
public:
	struct VertexShaderEntry
	{
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
		std::vector<uint8_t> bytecode;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};

	struct PixelShaderEntry
	{
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	};

	struct GeometryShaderEntry
	{
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs;
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// デバイスのポインタ
	ID3D11Device1* m_pDevice;

	//シェーダー
	std::unordered_map<std::string, VertexShaderEntry> m_vertexShaders;
	std::unordered_map<std::string, PixelShaderEntry> m_pixelShaders;
	std::unordered_map<std::string, GeometryShaderEntry> m_geometryShaders;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ShaderManager(ID3D11Device1* device);

	// デストラクタ
	~ShaderManager();

// 操作
public:
	// シェーダーを読込・作成
	bool CreateVS(const std::string& name, const wchar_t* filePath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc);
	bool CreatePS(const std::string& name, const wchar_t* filePath);
	bool CreateGS(const std::string& name, const wchar_t* filePath);


// 取得/設定
public:
	// シェーダーの取得
	VertexShaderEntry* GetVS(const std::string& name);
	PixelShaderEntry* GetPS(const std::string& name);
	GeometryShaderEntry* GetGS(const std::string& name);

// 内部実装
private:
	bool LoadFile(const wchar_t* filePath, std::vector<uint8_t>& out);
};
