/**
 * @file   UIManager.cpp
 *
 * @brief  UIマネージャーに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "UIManager.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Common/KeyConverter.h"
#include "Source/Game/UI/Elements/UIDimmer.h"
#include "Source/Game/UI/Elements/UIWidget.h"
#include "Source/Game/UI/Controls/AttackUI.h"
#include "Source/Game/UI/Controls/InputGuideUI.h"
#include "Source/Game/UI/Displays/StageResultUI.h"
#include "Source/Game/UI/Displays/PauseUI.h"
#include "Source/Game/UI/Displays/InputHintUI.h"


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
 * @param pDR			 デバイスリソースのポインタ
 * @param pKbTracker	 キーボードトラッカーのポインタ
 * @param conditionsType クリア条件
 * @param keyConfig		 操作キー設定
 *
 * @return なし
 */
void UIManager::SetupStageUI(
	DX::DeviceResources* pDR,
	DirectX::Keyboard::KeyboardStateTracker* pKbTracker,
	ClearConditionsUI::ConditionsType conditionsType,
	const InputKeyLoader::InputKeyInfo& keyConfig)
{
	// ディマーを作成
	m_dimmer = std::make_unique<UIDimmer>(pDR);

	// テクスチャの取得
	LoadTextures();

	// UIの作成
	// リザルト
	CreateResultUI();

	// クリア条件
	CreateClearConditionUI(conditionsType);

	// ポーズ画面
	CreatePauseUI();

	// 操作ガイド
	// 攻撃
	std::vector<DirectX::Keyboard::Keys> attack_keys;
	attack_keys.push_back(keyConfig.attack);
	CreateKeyGuideUI(DirectX::SimpleMath::Vector2(900, 600), attack_keys, pKbTracker);
	// 移動
	DirectX::SimpleMath::Vector2 base_pos = { 1130, 600 };
	float adjust_pos = 80;
	// 前
	std::vector<DirectX::Keyboard::Keys> forward_keys;
	forward_keys.push_back(keyConfig.move_forward);
	CreateKeyGuideUI({ base_pos.x, base_pos.y - adjust_pos }, forward_keys, pKbTracker);
	// 後ろ
	std::vector<DirectX::Keyboard::Keys> backward_keys;
	backward_keys.push_back(keyConfig.move_backward);
	CreateKeyGuideUI({ base_pos.x, base_pos.y + adjust_pos }, backward_keys, pKbTracker);
	// 左
	std::vector<DirectX::Keyboard::Keys> left_keys;
	left_keys.push_back(keyConfig.move_left);
	CreateKeyGuideUI({ base_pos.x - adjust_pos, base_pos.y }, left_keys, pKbTracker);
	// 右
	std::vector<DirectX::Keyboard::Keys> right_keys;
	right_keys.push_back(keyConfig.move_right);
	CreateKeyGuideUI({ base_pos.x + adjust_pos, base_pos.y }, right_keys, pKbTracker);

	// 攻撃方法
	OperationUI::Textures opTextures;	// テクスチャ
	opTextures.nomalArrow = m_textures->arrow_normal.texture;
	opTextures.rotateArrow = m_textures->arrow_rotate.texture;
	//opTextures.keyText = m_textures->text_operation.texture;
	opTextures.keyText = m_textures->text_keys.texture;
	OperationUI::InputKeys opKeys;		// 操作キー
	opKeys.change = keyConfig.mode_switch;
	opKeys.left = keyConfig.rotate_left;
	opKeys.right = keyConfig.rotate_right;
	OperationUI::OperationUIDesc opUIDesc =
	{
		opKeys,
		opTextures,
		m_textures->arrow_normal.size,
		m_textures->arrow_rotate.size,
		m_textures->text_keys.size.x,
		m_textures->icon_camera.size
	};
	opUIDesc.arrowRotateAdjustPos = DirectX::SimpleMath::Vector2(0.0f, -30.0f);
	opUIDesc.UIScale = 0.8f;
	CreateAttackUI(opUIDesc);

	// カメラ回転
	opTextures.icon = m_textures->icon_camera.texture;
	opUIDesc.textures = opTextures;
	opUIDesc.arrowRotateAdjustPos = DirectX::SimpleMath::Vector2(0.0f, -70.0f);
	opUIDesc.UIScale = 0.8f;
	CreateCameraUI(opUIDesc, DirectX::SimpleMath::Vector2(1080.0f, 130.0f));
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

	// リザルトUIの更新
	if (m_resultUI) m_resultUI->Update(elapsedTime);

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
	// ディマーより下
	context.spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp());

	// 基本UIの描画
	for (auto& ui : m_elements)
	{
		ui->Draw(context);
	}

	// 攻撃方法UIの描画
	m_attackUI->Draw(context);

	// カメラ回転UIの描画
	m_cameraUI->Draw(context);

	context.spriteBatch->End();

	// ディマーを描画
	if (m_resultUI->IsEnable()) DrawDimmer(m_resultUI.get(), context);
	if (m_pauseUI->IsOpen()) DrawDimmer(m_pauseUI.get(), context);

	// ディマーより上
	context.spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		context.states->NonPremultiplied(),
		context.states->LinearClamp());

	// リザルトUIの描画
	m_resultUI->Draw(context);

	// ポーズ画面UIの描画
	m_pauseUI->Draw(context);

	context.spriteBatch->End();
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

	// リザルトUIの終了
	if (m_resultUI) m_resultUI->Finalize();
	m_resultUI.reset();

	// 攻撃方法UIの終了
	if (m_attackUI) m_attackUI->Finalize();
	m_attackUI.reset();

	// カメラ回転UIの終了
	if (m_cameraUI) m_cameraUI->Finalize();
	m_cameraUI.reset();

	// ポーズ画面UIの終了
	if (m_pauseUI) m_pauseUI->Finalize();
	m_pauseUI.reset();

	// ディマーの終了
	m_dimmer.reset();
}



/**
 * @brief テクスチャの読み込み
 *
 * @param なし
 *
 * @return なし
 */
void UIManager::LoadTextures()
{
	m_textures = std::make_unique<UITextures>();

	// アイコン
	m_textures->icon_attackBasic = {
		m_pRM->RequestPNG("attack_basic", "UI/basicAtk.png"),
		TEX_SIZE_ICON_ATK };
	m_textures->icon_attackRolling = {
		m_pRM->RequestPNG("attack_rolling", "UI/rollingAtk.png"),
		TEX_SIZE_ICON_ATK };
	m_textures->icon_attackHeavy = {
		m_pRM->RequestPNG("attack_heavy", "UI/heavyAtk.png"),
		TEX_SIZE_ICON_ATK };
	m_textures->icon_camera = {
		m_pRM->RequestPNG("camera", "UI/camera.png"),
		TEX_SIZE_ICON_CAM };

	// ウィンドウ
	m_textures->window_result = {
		m_pRM->RequestPNG("resultPanel", "UI/resultPanel.png"),
		TEX_SIZE_WINDOW_RESULT };
	m_textures->window_pause = {
		m_pRM->RequestPNG("pauseWindow", "UI/pauseWindow.png"),
		TEX_SIZE_WINDOW_PAUSE };

	// 矢印
	m_textures->arrow_normal = {
		m_pRM->RequestPNG("arrow", "Resources/Textures/UI/arrow_triangle.png"),
		TEX_SIZE_ARROW_NOMAL };
	m_textures->arrow_rotate = {
		m_pRM->RequestPNG("rotate", "Resources/Textures/UI/arrow_rotate.png"),
		TEX_SIZE_ARROW_ROTATE };

	// 文字
	m_textures->text_operation = {
		m_pRM->RequestPNG("operationText", "Resources/Textures/text/operationText.png"),
		TEX_SIZE_TEXT_OPERATION };
	m_textures->text_conditions = {
		m_pRM->RequestPNG("conditionsText", "Text/conditionsText.png"),
		TEX_SIZE_TEXT_CONDITION };
	m_textures->text_keys = {
		m_pRM->RequestPNG("keysText", "Text/keysText.png"),
		TEX_SIZE_TEXT_KEYS };
	m_textures->text_pauseOptions = {
		m_pRM->RequestPNG("pauseOptionsText", "Text/pauseOptionsText.png"),
		TEX_SIZE_TEXT_OPTION };
	m_textures->text_pauseTitle = {
		m_pRM->RequestPNG("pauseTitleText", "Text/pauseTitleText.png"),
		TEX_SIZE_TEXT_TITLE };
	m_textures->text_number = {
		m_pRM->RequestPNG("number", "Text/number_48.png"),
		TEX_SIZE_TEXT_NUMBER };
	m_textures->text_clearTime = {
		m_pRM->RequestPNG("clearTimeText", "Text/clearTimeText.png"),
		TEX_SIZE_TEXT_CLEARTIME };

	// その他
	m_textures->base_key = {
		m_pRM->RequestPNG("keyBase", "UI/key.png"),
		TEX_SIZE_KEY_BASE };
}



/**
 * @brief ディマーを描画
 *
 * @param ui	  ディマーを使用するUI
 * @param context 描画用構造体
 *
 * @return なし
 */
void UIManager::DrawDimmer(UIElement* ui, RenderContext context)
{
	// 不透明度を設定
	m_dimmer->SetOpacity(ui->GetAnimLinearProgress(), Easing::EaseType::OutQuart, DIMMER_DARKNESS);
	// ディマーを描画
	m_dimmer->Draw(context);
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
	// テクスチャ情報を設定
	StageResultUI::Textures textures;
	textures.result = m_textures->window_result;
	textures.clearTime = m_textures->text_clearTime;
	textures.number = m_textures->text_number;

	// UIを作成
	m_resultUI = std::make_unique<StageResultUI>();
	m_resultUI->Initialize(
		textures,
		m_windowSize
	);
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
		m_textures->text_keys.texture,
		m_textures->base_key.texture,
		position,
		{ m_textures->base_key.size.x, m_textures->base_key.size.y },
		m_textures->text_keys.size.x,
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
		opUIDesc.arrowNormalSize.x * 1.75f,
		false
	);
}



/**
 * @brief ポーズ画面UIの作成
 *
 * @param なし
 *
 * @return なし
 */
void UIManager::CreatePauseUI()
{
	PauseUI::PauseUIDesc desc{
		m_textures->window_pause.size,
		m_textures->text_pauseTitle.size
	};
	PauseUI::Textures pauseTex{
		m_textures->window_pause.texture,
		m_textures->text_pauseTitle.texture,
		m_textures->text_pauseOptions.texture
	};

	// UIを作成
	m_pauseUI = std::make_unique<PauseUI>();
	m_pauseUI->Initialize(m_windowSize, pauseTex, desc);
}



/**
 * @brief 操作方法UIの作成
 *
 * @param なし
 *
 * @return なし
 */
void UIManager::CreateInputHintUI()
{

	// UIを作成
	std::unique_ptr<InputHintUI> inputUI = std::make_unique<InputHintUI>();
	//inputUI->Initialize();
	m_inputHintUI.push_back(std::move(inputUI));
}
