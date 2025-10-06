/**
 * @file   CountArea.cpp
 *
 * @brief  CountAreaに関するソースファイル
 *
 * @author 制作者名
 *
 * @date   日付
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CountArea.h"

using namespace DirectX;


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
CountArea::CountArea(UserResources* ur)
	: m_mode(TriggerMode::ReachCount)
	, m_targetNum(0)
	, m_isTrigger(false)
	, m_armed(false)
	, m_insideList()
	, m_collisionHandle{ 0 }
{
	auto dr = ur->GetDeviceResources();
	auto rm = ur->GetResourceManager();

	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(dr->GetD3DDeviceContext(), { 1.0f, 1.0f, 1.0f }, true);

	m_numberBorad = std::make_unique<NumberRenderer3D>(
		SimpleMath::Vector2(48.f, 72.f),
		rm->RequestPNG("number", L"Resources/Textures/Text/number_48.png"),
		1,
		dr
	);
}



/**
 * @brief デストラクタ
 */
CountArea::~CountArea()
{
	m_geometricPrimitive.reset();
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CountArea::Initialize(CollisionManager* pCollisionManager,
						   DirectX::SimpleMath::Vector3 position, float x, float z,
						   std::function<void()> operation,TriggerMode mode, int targetNum)
{
	m_position = position;

	m_operation = operation;
	
	m_mode = mode;

	m_targetNum = targetNum;

	m_isTrigger = false;

	// 数字UIの作成
	m_numberBorad->Initialize(m_insideList.size());

	SimpleMath::Vector3 boradPos = { m_position.x, m_position.y + AREA_HALF_HEIGHT * 2.f, m_position.z };
	m_numberBorad->SetPosition(m_position);

	// 当たり判定の設定
	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(SimpleMath::Vector3(x, AREA_HALF_HEIGHT, z));
	m_collider.SetRotation(SimpleMath::Quaternion::Identity);

	// コリジョンマネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Trigger;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.isTrigger = true;
	desc.callback.onEnter =
		[this,pCollisionManager](uint32_t, uint32_t handle)
		{
			if (pCollisionManager->GetDesc(handle)->layer != CollisionManager::Layer::EnemyBody) return;
			
			// debug --------------------------------
			const auto* d = pCollisionManager->GetDesc(handle);
			auto epos = d->sphere->GetCenter();     // 敵の中心
			auto apos = m_collider.GetCenter();     // エリア中心
			auto ahl = m_collider.GetHalfLength();  // エリア半径
			char buf[256];
			sprintf_s(buf, "ENTER id=%u  enemy=(%.2f,%.2f,%.2f)  areaC=(%.2f,%.2f,%.2f)  areaHL=(%.2f,%.2f,%.2f)\n",
				d->userId, epos.x, epos.y, epos.z, apos.x, apos.y, apos.z, ahl.x, ahl.y, ahl.z);
			OutputDebugStringA(buf);
			// --------------------------------------

			// IDを取得
			uint32_t id = pCollisionManager->GetDesc(handle)->userId;
			if (id == 0) return;

			// リストに追加
			auto it = std::find(m_insideList.begin(), m_insideList.end(), id);
			if (it == m_insideList.end()) m_insideList.push_back(id);

			m_armed = true;

			// 条件を満たしているか
			if (m_mode == TriggerMode::ReachCount)
			{
				if (m_insideList.size() >= m_targetNum) m_isTrigger = true;
			}
		};
	desc.callback.onExit =
		[this,pCollisionManager](uint32_t, uint32_t handle)
		{
			if (pCollisionManager->GetDesc(handle)->layer != CollisionManager::Layer::EnemyBody) return;

			// debug --------------------------------
			const auto* d = pCollisionManager->GetDesc(handle);
			char buf[256];
			sprintf_s(buf,
				"EXIT : handle=%u layer=%d userId=%u  enemyCount(before)=%zu\n",
				handle, (int)d->layer, d->userId, m_insideList.size());
			OutputDebugStringA(buf);
			// --------------------------------------

			// IDを取得
			uint32_t id = pCollisionManager->GetDesc(handle)->userId;

			// リストから削除
			m_insideList.erase(std::remove(m_insideList.begin(), m_insideList.end(), id), m_insideList.end());

			// 条件を満たしているか
			if (m_mode == TriggerMode::AllOut)
			{
				if (m_insideList.size() == 0 && m_armed) m_isTrigger = true;
			}
		};
	m_collisionHandle = pCollisionManager->Add(desc);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CountArea::Update()
{
	if (m_isTrigger)
	{
		m_operation();
	}

	// 表示する数字の更新
	m_numberBorad->SetNumber(m_insideList.size());
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CountArea::Draw(RenderContext& context, Imase::DebugFont* debugFont)
{
	SimpleMath::Vector3 size = m_collider.GetHalfLength();
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(size.x * 2.0f, size.y * 2.0f, size.z * 2.0f);
	world = scale * trans;

	m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Magenta, nullptr, true);

	m_numberBorad->Draw(context);

	//debugFont->AddString(0, 170, Colors::Magenta, L" areaPos = %f,%f,%f", m_position.x, m_position.y, m_position.z);
	debugFont->AddString(0, 170, Colors::Magenta, L"enter = %d", m_insideList.size());
	debugFont->AddString(110, 170, Colors::Magenta, L"trigger = %d", m_isTrigger);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void CountArea::Finalize()
{
	m_geometricPrimitive.reset();

	if(m_numberBorad) m_numberBorad->Finalize();
	m_numberBorad.reset();
}
