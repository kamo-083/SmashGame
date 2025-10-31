/**
 * @file   StageScene.h
 *
 * @brief  ステージシーンに関するヘッダファイル
 */

// 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include "Source/Game/Common/Scene.h"
#include "Source/Game/UI/ClearConditionsUI.h"



// クラスの宣言 ===============================================================
class Camera;
class Player;
class EnemyManager;
class StageManager;
class EffectManager;
class CollisionManager;
class Sky;
class UIWidget;
class AttackUI;
class StageResultUI;
class OperationUI;


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
		ID3D11ShaderResourceView* key;		// 操作テキスト
	};

	const ClearConditionsUI::ConditionsType CLEAR_CONDITIONS;	// クリア条件

	// クリア条件UI
	// クリア条件テキストの1行分の大きさ
	static constexpr DirectX::SimpleMath::Vector2 CONDITIONS_TEXT_SIZE = { 300.f,90.f };

	// 操作方法UI共通
	// テキスト画像を切り取る時の左端
	static constexpr float TEXT_UV_LEFT = 370.0f;
	// テキスト画像の1文字の大きさ
	static constexpr DirectX::SimpleMath::Vector2 TEXT_SIZE = { 50.0f,100.0f };
	// 矢印画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 ARROW_SIZE_DEFAULT = { 200.0f,100.0f };	// 通常
	static constexpr DirectX::SimpleMath::Vector2 ARROW_SIZE_ROTATE = { 200.0f,200.0f };	// 回転
	
	// カメラ操作UI
	// アイコン画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 CAMERA_ICON_SIZE = { 200.0f,135.0f };
	// 表示位置
	static constexpr DirectX::SimpleMath::Vector2 CAMERA_UI_POS = { 250.0f,600.0f };
	// 矢印間隔
	static constexpr float CAMERA_UI_ARROW_INTERVAL = 350.0f;

	// 攻撃変更UI
	// 攻撃アイコン画像サイズ
	static constexpr DirectX::SimpleMath::Vector2 ATTACK_ICON_SIZE = { 200.0f,200.0f };

	// リザルトUI
	static constexpr DirectX::SimpleMath::Vector2 RESULT_WINDOW_SIZE = { 350.0f,400.0f };

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

	// 攻撃UI
	std::unique_ptr<AttackUI> m_attackUI;

	// リザルトUI
	std::unique_ptr<StageResultUI> m_resultUI;

	// ステージクリア条件UI
	std::unique_ptr<ClearConditionsUI> m_conditionsUI;

	// 操作方法UI
	std::unique_ptr<OperationUI> m_cameraUI;

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

	// UIの設定
	void SetupUI(DirectX::SimpleMath::Vector2 windowSize, ResourceManager* pRM);

	// テクスチャの設定
	void SetupTextures(ResourceManager* pRM);

	// 音声の設定
	void SetupSounds(AudioManager* pAM);
};
