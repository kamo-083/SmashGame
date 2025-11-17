/**
 * @file   StageScene.h
 *
 * @brief  ステージシーンに関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"
#include "Source/Game/Data/InputKeyLoader.h"
#include "Source/Game/UI/Displays/ClearConditionsUI.h"


// クラスの宣言 ===============================================================
class Camera;
class Player;
class EnemyManager;
class StageManager;
class EffectManager;
class UIManager;
class CollisionManager;
class Sky;
class UIWidget;
class AttackUI;
class StageResultUI;
class OperationUI;
class InputGuideUI;


// クラスの定義 ===============================================================
/**
 * @brief ステージシーン
 */
class StageScene : public Scene
{
	// クラス定数の宣言 -------------------------------------------------
public:
	// シーンの状態
	enum class Overlay
	{
		NONE = -1,
		GAMEPLAY,	// ゲーム
		RESULT,		// リザルト
	};

	// テクスチャ群
	struct Textures
	{
		ID3D11ShaderResourceView* shadow;	// 丸影
	};

	const ClearConditionsUI::ConditionsType CLEAR_CONDITIONS;	// クリア条件

	// BGMの音量
	static constexpr float BGM_VOLUME = 0.5f;

	// 影用の頂点数
	static constexpr int SHADOW_VERTEX_NUM = 4;
	// 影用の高さ調整
	static constexpr float SHADOW_HEIGHT_ADJUST = 0.01f;


	// データメンバの宣言 -----------------------------------------------
private:
	// システム関連
	// 切り替え操作のモード true:攻撃 false:カメラ
	bool m_keyMode;

	// 当たり判定マネージャー
	std::unique_ptr<CollisionManager> m_collisionManager;

	// エフェクトマネージャー
	std::unique_ptr<EffectManager> m_effectManager;

	// オーバーレイ
	Overlay m_overlayMode;

	// 操作キー設定
	InputKeyLoader::InputKeyInfo m_keyConfig;

	// 影関連
	// プリミティブバッチ
	std::unique_ptr<DirectX::DX11::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// ベーシックエフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// 深度ステンシルステート
	// ステージオブジェクト用
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState_stage;
	// キャラクター用
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState_shadow;


	// オブジェクト関連
	// カメラ
	std::unique_ptr<Camera> m_camera;

	// プレイヤー
	std::unique_ptr<Player> m_player;

	// 敵マネージャー
	std::unique_ptr<EnemyManager> m_enemyManager;

	// ステージマネージャー
	std::unique_ptr<StageManager> m_stageManager;

	// UIマネージャー
	std::unique_ptr<UIManager> m_UIManager;

	// スカイドーム
	std::unique_ptr<Sky> m_sky;

	// ステージファイルへのパス
	std::string m_stageFilePath;

	// テクスチャ群
	std::unique_ptr<Textures> m_textures;


// メンバ関数の宣言 -------------------------------------------------
// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	StageScene(
		SceneManager* pSM, UserResources* pUR,
		std::string path, ClearConditionsUI::ConditionsType clearCondition);

	// デストラクタ
	~StageScene();


// 操作
public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(RenderContext context, DebugFont* debugFont) override;

	// 終了処理
	void Finalize() override;
	
	// SEの再生
	void PlaySE(std::string seName);


// 取得/設定
public:


// 内部実装
private:
	// 深度ステンシルステートの作成
	void CreateDepthStencilState(ID3D11Device* device);

	// 影の設定
	void SettingShadow(RenderContext context);

	// 影の描画
	void DrawShadow(const DirectX::SimpleMath::Vector3 position, const float radius);

	// 影用の頂点を作成
	std::array<DirectX::VertexPositionTexture, SHADOW_VERTEX_NUM> CreateVertexes(
		const DirectX::SimpleMath::Vector3 position, const float radius);

	// オブジェクトの影を描画
	void DrawObjectsShadow(RenderContext context);

	// リザルト中の更新
	void UpdateResult(float elapsedTime);

	// キー操作モードの切り替え
	void ChangeKeyMode();


// 初期設定関連
	// 当たり判定のレイヤーフィルターの設定
	void SetupCollitionLayer();

	// エフェクトマネージャーの設定
	void SetupEffects(DX::DeviceResources* pDR);

	// プレイヤーの設定
	void SetupPlayer(ResourceManager* pRM);

	// 敵の設定
	void SetupEnemy();

	// ステージの設定
	void SetupStage();

	// スカイドームの設定
	void SetupSkydome();

	// テクスチャの設定
	void SetupTextures(ResourceManager* pRM);

	// 音声の設定
	void SetupSounds(AudioManager* pAM);
};
