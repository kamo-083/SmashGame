/**
 * @file   ShaderManager.h
 *
 * @brief  シェーダーマネージャーに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include<unordered_map>
#include<string>
#include<vector>


// クラスの定義 ===============================================================
/**
 * @brief シェーダーマネージャー
 */
class ShaderManager
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// 頂点シェーダーの情報
	struct VertexShaderEntry
	{
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
		std::vector<uint8_t> bytecode;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	};

	// ピクセルシェーダーの情報
	struct PixelShaderEntry
	{
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	};

	// ジオメトリシェーダーの情報
	struct GeometryShaderEntry
	{
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs;
	};


	// データメンバの宣言 -----------------------------------------------
private:
	// デバイスのポインタ
	ID3D11Device1* m_pDevice;

	// シェーダーの配列
	std::unordered_map<std::string, VertexShaderEntry> m_vertexShaders;		// 頂点シェーダー
	std::unordered_map<std::string, PixelShaderEntry> m_pixelShaders;		// ピクセルシェーダー
	std::unordered_map<std::string, GeometryShaderEntry> m_geometryShaders;	// ジオメトリシェーダー


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
	bool CreateVS(
		const std::string& name, const std::string& filePath,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc);	// 頂点シェーダー
	bool CreatePS(const std::string& name, const std::string& filePath);	// ピクセルシェーダー
	bool CreateGS(const std::string& name, const std::string& filePath);	// ジオメトリシェーダー


// 取得/設定
public:
	// シェーダーの取得
	VertexShaderEntry* GetVS(const std::string& name);		// 頂点シェーダー
	PixelShaderEntry* GetPS(const std::string& name);		// ピクセルシェーダー
	GeometryShaderEntry* GetGS(const std::string& name);	// ジオメトリシェーダー

// 内部実装
private:
	// ファイルの読み込み
	bool LoadFile(const wchar_t* filePath, std::vector<uint8_t>& out);

	// string型をwchar_t型へ変換
	std::wstring StringToWchar(std::string str);

};
