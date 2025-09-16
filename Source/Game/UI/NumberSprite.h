/**
 * @file   NumberSprite.h
 *
 * @brief  NumberSpriteに関するヘッダファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================



// クラスの定義 ===============================================================
/**
 * @brief NumberSprite
 */
class NumberSprite
{
	// クラス定数の宣言 -------------------------------------------------
public:
	const DirectX::SimpleMath::Vector2 SPRITE_SIZE;

	const int NUM_DIGIT;


	// データメンバの宣言 -----------------------------------------------
private:
	// 表示する数字
	int m_number;

	// テクスチャ
	ID3D11ShaderResourceView* m_texture;

	// 表示位置
	DirectX::SimpleMath::Vector2 m_position;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	NumberSprite(DirectX::SimpleMath::Vector2 spriteSize, ID3D11ShaderResourceView* texture, int digit);

	// デストラクタ
	~NumberSprite();


// 操作
public:
	// 初期化処理
	void Initialize(DirectX::SimpleMath::Vector2 position, const int& number);

	// 描画処理
	void Draw(DirectX::SpriteBatch& spriteBatch);

	// 終了処理
	void Finalize();

// 取得/設定
public:
	// 数値を設定
	void SetNumber(const int& number) { m_number = number; }

	// 座標を設定
	void SetPosition(const DirectX::SimpleMath::Vector2& pos) { m_position = pos; }

// 内部実装
private:

};
