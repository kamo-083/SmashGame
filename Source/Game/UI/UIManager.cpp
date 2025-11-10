/**
 * @file   UIManager.cpp
 *
 * @brief  UIマネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "UIManager.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/UI/Elements/UIWidget.h"
#include "Source/Game/UI/Controls/AttackUI.h"
#include "Source/Game/UI/Controls/InputGuideUI.h"
#include "Source/Game/UI/Displays/StageResultUI.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
UIManager::UIManager(
	DirectX::SimpleMath::Vector2 windowSize,
	ResourceManager* pRM)
	: m_windowSize(windowSize)
	, m_pRM(pRM)
{

}



/**
 * @brief デストラクタ
 */
UIManager::~UIManager()
{
	m_textures.reset();
}



/**
 * @brief UIを初期化(ステージ用)
 *
 * @param pKbTracker	 キーボードトラッカーのポインタ
 * @param conditionsType クリア条件
 *
 * @return なし
 */
void UIManager::SetupStageUI(
	DirectX::Keyboard::KeyboardStateTracker* pKbTracker,
	ClearConditionsUI::ConditionsType conditionsType)
{
	// テクスチャの取得
	m_textures = std::make_unique<UITextures>();
	// アイコン
	m_textures->icon_attackBasic = {
		m_pRM->RequestPNG("attack_basic", "UI/basicAtk.png"),
		DirectX::SimpleMath::Vector2(200.0f, 200.0f) };
	m_textures->icon_attackRolling = {
		m_pRM->RequestPNG("attack_rolling", "UI/rollingAtk.png"),
		DirectX::SimpleMath::Vector2(200.0f, 200.0f) };
	m_textures->icon_attackHeavy = {
		m_pRM->RequestPNG("attack_heavy", "UI/heavyAtk.png"),
		DirectX::SimpleMath::Vector2(200.0f, 200.0f) };
	m_textures->icon_camera = {
			m_pRM->RequestPNG("camera", "UI/camera.png"),
			DirectX::SimpleMath::Vector2(200.0f, 135.0f) };
	// ウィンドウ
	m_textures->window_result = {
		m_pRM->RequestPNG("resultPanel", "UI/resultPanel.png"),
		DirectX::SimpleMath::Vector2(350.0f, 400.0f) };
	// 矢印
	m_textures->arrow_normal = {
		m_pRM->RequestPNG("arrow", "Resources/Textures/UI/arrow_triangle.png"),
		DirectX::SimpleMath::Vector2(200.0f, 100.0f) };
	m_textures->arrow_rotate = {
		m_pRM->RequestPNG("rotate", "Resources/Textures/UI/arrow_rotate.png"),
		DirectX::SimpleMath::Vector2(200.0f, 200.0f) };
	// 文字
	m_textures->text_operation = {
		m_pRM->RequestPNG("operationText", "Resources/Textures/text/operationText.png"),
		DirectX::SimpleMath::Vector2(50.0f, 100.0f) };
	m_textures->text_conditions = {
		m_pRM->RequestPNG("conditionsText", "Text/conditionsText.png"),
		DirectX::SimpleMath::Vector2(300.0f, 90.0f) };

	// UIの作成
	// リザルト
	CreateResultUI();
	// クリア条件
	CreateClearConditionUI(conditionsType);
	// 操作ガイド
	std::vector<DirectX::Keyboard::Keys> move_keys;
	move_keys.push_back(DirectX::Keyboard::Up);
	move_keys.push_back(DirectX::Keyboard::Down);
	move_keys.push_back(DirectX::Keyboard::Right);
	move_keys.push_back(DirectX::Keyboard::Left);
	CreateKeyGuideUI(DirectX::SimpleMath::Vector2(100, 600), move_keys, pKbTracker);
	// 攻撃方法
	OperationUI::Textures opTextures;
	opTextures.nomalArrow = m_textures->arrow_normal.texture;
	opTextures.rotateArrow = m_textures->arrow_rotate.texture;
	opTextures.keyText = m_textures->text_operation.texture;
	OperationUI::OperationUIDesc opUIDesc =
	{
		opTextures,
		 m_textures->arrow_normal.size,
		m_textures->arrow_rotate.size,
		370.0f,
		m_textures->text_operation.size,
		m_textures->icon_camera.size
	};
	CreateAttackUI(opUIDesc);
	// カメラ回転
	CreateCameraUI(opUIDesc, DirectX::SimpleMath::Vector2(250.0f, 90.0f));
}



/**
 * @brief 更新処理
 *
 * @param elapsedTime	経過時間
 *
 * @return なし
 */
void UIManager::Update(float elapsedTime)
{
	// 基本UIの更新
	for (auto& ui : m_elements)
	{
		ui->Update(elapsedTime);
	}

	// 攻撃方法UIの更新
	if (m_attackUI) m_attackUI->Update(elapsedTime);

	// カメラ回転UIの更新
	if (m_cameraUI) m_cameraUI->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 *
 * @return なし
 */
void UIManager::Draw(RenderContext context)
{
	// 基本UIの描画
	for (auto& ui : m_elements)
	{
		ui->Draw(context);
	}

	// 攻撃方法UIの描画
	if (m_attackUI) m_attackUI->Draw(context);

	// カメラ回転UIの描画
	if (m_cameraUI) m_cameraUI->Draw(context);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void UIManager::Finalize()
{
	// 基本UIの終了
	for (auto& ui : m_elements)
	{
		ui->Finalize();
	}
	m_elements.clear();

	// 攻撃方法UIの終了
	if (m_attackUI) m_attackUI->Finalize();
	m_attackUI.reset();

	// カメラ回転UIの終了
	if (m_cameraUI) m_cameraUI->Finalize();
	m_cameraUI.reset();
}



/**
 * @brief リザルトUIの作成
 *
 * @param なし
 *
 * @return なし
 */
void UIManager::CreateResultUI()
{
	// UIを作成
	std::unique_ptr<StageResultUI> resultUI = std::make_unique<StageResultUI>();
	resultUI->Initialize(
		m_textures->window_result.texture,
		m_textures->window_result.size,
		m_windowSize
	);

	// 配列に追加
	m_elements.push_back(std::move(resultUI));
}



/**
 * @brief クリア条件UIの作成
 *
 * @param type	クリア条件
 *
 * @return なし
 */
void UIManager::CreateClearConditionUI(ClearConditionsUI::ConditionsType type)
{
	// UIを作成
	std::unique_ptr<ClearConditionsUI> conditionUI = std::make_unique<ClearConditionsUI>(type);
	conditionUI->Initialize(
		m_windowSize,
		m_textures->text_conditions.texture,
		m_textures->text_conditions.size
	);

	// 配列に追加
	m_elements.push_back(std::move(conditionUI));
}



/**
 * @brief 操作ガイドUIの作成
 *
 * @param position   表示位置
 * @param key	     対応キー
 * @param pKbTracker キーボードトラッカーのポインタ
 *
 * @return なし
 */
void UIManager::CreateKeyGuideUI(
	DirectX::SimpleMath::Vector2 position,
	std::vector<DirectX::Keyboard::Keys> keys, 
	DirectX::Keyboard::KeyboardStateTracker* pKbTracker)
{
	// UIを作成
	std::unique_ptr<InputGuideUI> guideUI = std::make_unique<InputGuideUI>();
	guideUI->Initialize(
		m_textures->arrow_normal.texture,
		position,
		m_textures->arrow_normal.size,
		keys,
		pKbTracker
	);

	// 配列に追加
	m_elements.push_back(std::move(guideUI));
}



/**
 * @brief 攻撃方法UIの作成
 *
 * @param opUIDesc	操作UIの設定情報
 *
 * @return なし
 */
void UIManager::CreateAttackUI(OperationUI::OperationUIDesc opUIDesc)
{
	AttackUI::AttackUIDesc atkUIDesc =
	{
		m_textures->icon_attackBasic.texture,
		m_textures->icon_attackRolling.texture,
		m_textures->icon_attackHeavy.texture,
		m_textures->icon_attackBasic.size.x,
		m_textures->icon_attackBasic.size.y
	};

	// UIを作成
	m_attackUI = std::make_unique<AttackUI>(m_windowSize.x, m_windowSize.y);
	m_attackUI->Initialize(atkUIDesc, opUIDesc);
}



/**
 * @brief カメラ回転UIの作成
 *
 * @param opUIDesc	操作UIの設定情報
 * @param position	表示位置
 *
 * @return なし
 */
void UIManager::CreateCameraUI(
	OperationUI::OperationUIDesc opUIDesc, 
	DirectX::SimpleMath::Vector2 position)
{
	// UIを作成
	m_cameraUI = std::make_unique<OperationUI>();
	m_cameraUI->Initialize(
		opUIDesc,
		position,
		opUIDesc.arrowSizeDefault.x * 1.75f,
		false
	);
}
