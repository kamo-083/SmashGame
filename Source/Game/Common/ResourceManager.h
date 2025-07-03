/**
 * @file   ResourceManager.h
 *
 * @brief  リソースマネージャに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once




// ヘッダファイルの読み込み ===================================================
#include "DeviceResources.h"
#include <unordered_map>




// クラスの定義 ===============================================================
/**
 * @brief リソースマネージャ
 */
class ResourceManager
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	//デバイスリソース
	ID3D11Device* m_pDevice;

	//テクスチャ
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;

	//モデル
	std::unordered_map<std::string, std::unique_ptr<DirectX::Model>> m_models;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ResourceManager(ID3D11Device* pDevice);

	// デストラクタ
	~ResourceManager();


// 操作
public:
	//PNGファイルの読み込み
	bool LoadPNG(const std::string& key, const wchar_t* filename);

	//DDSファイルの読み込み
	bool LoadDDS(const std::string& key, const wchar_t* filename);

	//テクスチャの取得
	ID3D11ShaderResourceView* GetTexture(const std::string& key);

	//PNGファイルの要求(キーがあったら渡す、無かったら読み込んでから渡す)(読み込みはPNGしか対応してないです)
	ID3D11ShaderResourceView* RequestTexture(const std::string& key, const wchar_t* filename);

	//モデルの読み込み
	bool LoadSDKMESH(const std::string& key, const wchar_t* filename);

	//モデルの取得
	DirectX::Model* GetModel(const std::string& key);

	//モデルの要求(キーがあったら渡す、無かったら読み込んでから渡す)
	DirectX::Model* RequestSDKMESH(const std::string& key, const wchar_t* filename);

// 内部実装
private:


};
