/**
 * @file   UITextureCatalog.cpp
 *
 * @brief  UI使用画像のカタログに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "UITextureCatalog.h"
#include "Source/Game/Common/ResourceManager.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
UITextureCatalog::UITextureCatalog()
	: m_loaded(false)
{

}



/**
 * @brief デストラクタ
 */
UITextureCatalog::~UITextureCatalog()
{
	m_textures.reset();
}



/**
 * @brief 画像の読み込み
 *
 * @param pRM リソースマネージャーのポインタ
 * 
 * @return なし
 */
void UITextureCatalog::Load(ResourceManager* pRM)
{
	// 読み込み済みならここで終了
	if (m_loaded) return;

	m_textures = std::make_unique<UITextures>();

	// アイコン
	m_textures->icon_attackBasic = {
		pRM->RequestPNG("attack_basic", "UI/basicAtk.png"),
		TexturesSize::ICON_ATK };
	m_textures->icon_attackRolling = {
		pRM->RequestPNG("attack_rolling", "UI/rollingAtk.png"),
		TexturesSize::ICON_ATK };
	m_textures->icon_attackHeavy = {
		pRM->RequestPNG("attack_heavy", "UI/heavyAtk.png"),
		TexturesSize::ICON_ATK };
	m_textures->icon_camera = {
		pRM->RequestPNG("camera", "UI/camera.png"),
		TexturesSize::ICON_CAM };

	// ウィンドウ
	m_textures->window_result = {
		pRM->RequestPNG("resultWindow", "UI/resultWindow.png"),
		TexturesSize::WINDOW_RESULT };
	m_textures->window_pause = {
		pRM->RequestPNG("pauseWindow", "UI/pauseWindow.png"),
		TexturesSize::WINDOW_PAUSE };

	// 矢印
	m_textures->arrow_normal = {
		pRM->RequestPNG("arrow", "Resources/Textures/UI/arrow_triangle.png"),
		TexturesSize::ARROW_NOMAL };
	m_textures->arrow_rotate = {
		pRM->RequestPNG("rotate", "Resources/Textures/UI/arrow_rotate.png"),
		TexturesSize::ARROW_ROTATE };

	// 文字
	m_textures->text_conditions = {
		pRM->RequestPNG("conditionsText", "Text/conditionsText.png"),
		TexturesSize::TEXT_CONDITION };
	m_textures->text_keys = {
		pRM->RequestPNG("keysText", "Text/keysText.png"),
		TexturesSize::TEXT_KEYS };
	m_textures->text_pauseOptions = {
		pRM->RequestPNG("pauseOptionsText", "Text/pauseOptionsText.png"),
		TexturesSize::TEXT_OPTION };
	m_textures->text_pauseTitle = {
		pRM->RequestPNG("pauseTitleText", "Text/pauseTitleText.png"),
		TexturesSize::TEXT_TITLE };
	m_textures->text_number = {
		pRM->RequestPNG("number", "Text/number_48.png"),
		TexturesSize::TEXT_NUMBER };
	m_textures->text_clearTime = {
		pRM->RequestPNG("clearTimeText", "Text/clearTimeText.png"),
		TexturesSize::TEXT_CLEARTIME };

	// その他
	m_textures->base_key = {
		pRM->RequestPNG("keyBase", "UI/key.png"),
		TexturesSize::KEY_BASE };

	// 読み込み済みに設定
	m_loaded = true;
}
