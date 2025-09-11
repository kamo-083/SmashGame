/**
 * @file   StagePanel.cpp
 *
 * @brief  StagePanelに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "StagePanel.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
StagePanel::StagePanel(int stage_num)
	:STAGE_NUM{ stage_num }
{

}



/**
 * @brief デストラクタ
 */
StagePanel::~StagePanel()
{

}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StagePanel::Initialize(ID3D11ShaderResourceView* texture,
							DirectX::SimpleMath::Vector2 texSize,
							DirectX::SimpleMath::Vector2 windowSize)
{
	m_UI = std::make_unique<UIWidget>();
	//m_UI->Initialize();
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StagePanel::Update(float elapsedTime)
{

}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StagePanel::Draw(RenderContext context)
{

}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void StagePanel::Finalize()
{

}
