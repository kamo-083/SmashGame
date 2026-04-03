/**
 * @file   ResourceManager.h
 *
 * @brief  リソースマネージャに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "DeviceResources.h"
#include "Source/Game/Common/Animation.h"
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

	//アニメーション
	std::unordered_map<std::string, std::unique_ptr<DX::AnimationSDKMESH>> m_animations;

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
	bool LoadPNG(const std::string& key, const std::string& filename);

	//DDSファイルの読み込み
	bool LoadDDS(const std::string& key, const std::string& filename);

	//テクスチャの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(const std::string& key);

	//PNGファイルの要求(キーがあったら渡す、無かったら読み込んでから渡す)
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RequestPNG(const std::string& key, const std::string& filename);

	//DDSファイルの要求(キーがあったら渡す、無かったら読み込んでから渡す)
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RequestDDS(const std::string& key, const std::string& filename);

	//モデルの読み込み
	bool LoadSDKMESH(const std::string& key, const std::string& filename, bool anim = false);

	//モデルの取得
	DirectX::Model* GetModel(const std::string& key);

	//モデルの要求(キーがあったら渡す、無かったら読み込んでから渡す)
	DirectX::Model* RequestSDKMESH(const std::string& key, const std::string& filename, bool anim = false);

	//アニメーションの読み込み
	bool LoadAnimation(const std::string& key, const std::string& filename);

	//アニメーションの取得
	DX::AnimationSDKMESH* GetAnimation(const std::string& key);
	
	//アニメーションの要求(キーがあったら渡す、無かったら読み込んでから渡す)
	DX::AnimationSDKMESH* RequestAnimation(const std::string& key, const std::string& filename);

// 内部実装
private:
	// string型をwchar_t型へ変換
	std::wstring StringToWchar(const std::string& str);

	// ファイルパスの作成
	std::string ResolveFilePath_Tex(const std::string& filename);		// テクスチャ用
	std::string ResolveFilePath_Model(const std::string& filename);		// モデル用
	std::string ResolveFilePath_Anim(const std::string& filename);		// アニメーション用
};