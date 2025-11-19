/**
 * @file   CountArea.h
 *
 * @brief  エリアに関するヘッダファイル
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include <functional>
#include"Source/Debug/DebugFont.h"
#include"Source/Game/Common/UserResources.h"
#include"Source/Game/Physics/Collision.h"
#include"Source/Game/Physics/CollisionManager.h"
#include"Source/Game/Common/ShaderManager.h"
#include"Source/Game/Common/RenderContext.h"
#include"Source/Game/UI/Elements/NumberRenderer/NumberRenderer3D.h"


// クラスの定義 ===============================================================
/**
 * @brief エリア
 */
class CountArea
{
	// クラス定数の宣言 -------------------------------------------------
public:
	static constexpr float AREA_HALF_HEIGHT = 1.0f;	// エリアの高さ(半分)

	// 条件
	enum class TriggerMode
	{
		ReachCount,	// 目標数以上入れる
		AllOut		// 全部外に出す
	};

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;	// インプットレイアウト

	// コンストバッファ
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix	 matWorld;	// ワールド行列
		DirectX::SimpleMath::Matrix	 matView;	// ビュー行列
		DirectX::SimpleMath::Matrix	 matProj;	// 射影行列
		DirectX::SimpleMath::Vector4 Diffuse;	// 基本色
		float Height = 0.0f;					// 高さ
		float Time = 0.0f;						// 時間
		DirectX::SimpleMath::Vector2 Dummy;		// ダミーデータ
	};

	// スプライト数字の1文字分のサイズ
	static constexpr DirectX::SimpleMath::Vector2 NUMBER_SIZE = { 48.0f,72.0f };


	// データメンバの宣言 -----------------------------------------------
private:
	// 座標
	DirectX::SimpleMath::Vector3 m_position;

	// モード
	TriggerMode m_mode;

	// 目標数
	int m_targetNum;

	// 目標に達しているか
	bool m_isTrigger;

	// 内部にいるか(AllOut用)
	bool m_armed;

	// エリア内にいる敵IDのリスト
	std::vector<uint32_t> m_insideList;

	// 条件を達成したときの動作
	std::function<void()> m_operation;

	// 当たり判定
	OBBCollider m_collider;

	// 衝突判定のハンドル
	uint32_t m_collisionHandle;

	// 累計経過時間
	float m_timer;

	// バッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;

	// シェーダー
	ShaderManager::VertexShaderEntry* m_vs;		// 頂点シェーダー
	ShaderManager::PixelShaderEntry* m_ps;		// ピクセルシェーダー
	ShaderManager::GeometryShaderEntry* m_gs;	// ジオメトリシェーダー

	std::unique_ptr<DirectX::GeometricPrimitive> m_geometricPrimitive;

	std::unique_ptr<NumberRenderer3D> m_numberBorad;	// 内部の敵数描画

	// エリアの表示色
	DirectX::SimpleMath::Color m_color;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	CountArea(UserResources* pUR);

	// デストラクタ
	~CountArea();


// 操作
public:
	// 初期化処理
	void Initialize(
		CollisionManager* pCM,
		DirectX::SimpleMath::Vector3 position, float x, float z,
		std::function<void()> operation, TriggerMode mode, int targetNum = 0);

	// 更新処理
	void Update(float elapsedTime,DirectX::SimpleMath::Vector3 cameraPos, DirectX::SimpleMath::Vector3 cameraUp);

	// 描画処理
	void Draw(RenderContext& context, DebugFont* debugFont);

	// 終了処理
	void Finalize();


// 取得/設定
public:

// 内部実装
private:
	// シェーダーの読み込み
	void LoadShaders(ShaderManager* shaderManager, ID3D11Device* device);

	// エリアを囲うポリゴンの描画
	void DrawArea(
		RenderContext& context,
		DirectX::SimpleMath::Matrix& world,
		DirectX::SimpleMath::Vector3& areaSize
		);

	// トリガーを起動
	void TriggerOn();
};
