/**
 * @file   CountArea.cpp
 *
 * @brief  エリアに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CountArea.h"
#include "Source/Game/Effect/Area/AreaEffect.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUR	ユーザーリソースのポインタ
 */
CountArea::CountArea(UserResources* pUR)
	:
	m_mode(TriggerMode::ReachCount),
	m_targetNum(0),
	m_isTrigger(false),
	m_armed(false),
	m_insideList(),
	m_collisionHandle(0)
{
	DX::DeviceResources* dr = pUR->GetDeviceResources();
	ResourceManager*	 rm = pUR->GetResourceManager();

	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(dr->GetD3DDeviceContext(), { 1.0f, 1.0f, 1.0f }, true);

	m_numberBorad = std::make_unique<NumberRenderer3D>(
		NUMBER_SIZE,
		rm->RequestPNG("number", "Text/number_48.png"),
		1,
		dr,
		0.5f
	);

	// エフェクトの作成
	m_effect = std::make_unique<AreaEffect>(pUR);
}



/**
 * @brief デストラクタ
 */
CountArea::~CountArea()
{
	m_effect.reset();
	m_geometricPrimitive.reset();
}



/**
 * @brief 初期化処理
 *
 * @param pCM		当たり判定マネージャーのポインタ
 * @param position	位置
 * @param x			X方向の長さ
 * @param z			Z方向の長さ
 * @param operation	条件達成時の処理
 * @param mode		条件
 * @param targetNum	目標数
 *
 * @return なし
 */
void CountArea::Initialize(
	CollisionManager* pCM,
	const DirectX::SimpleMath::Vector3& position, float x, float z,
	const std::function<void()>& operation, TriggerMode mode, int targetNum)
{
	// 位置を設定
	DirectX::SimpleMath::Vector3 pos = { position.x,position.y + AREA_HALF_HEIGHT,position.z };
	m_position = pos;

	// 条件達成時の処理を設定
	m_operation = operation;

	// 条件のモードを設定
	m_mode = mode;

	// 目標数を設定
	m_targetNum = targetNum;

	// トリガーをリセット
	m_isTrigger = false;

	// エフェクトの設定
	m_effect->SetPosition(DirectX::SimpleMath::Vector3(m_position.x, position.y, m_position.z));
	m_effect->SetLength(DirectX::SimpleMath::Vector3(x * 2.0f, AREA_HALF_HEIGHT * 2.0f, z * 2.0f));
	m_effect->SetColor(DirectX::Colors::Red.v);

	// 数字UIの作成
	m_numberBorad->Initialize(static_cast<int>(m_insideList.size()));
	DirectX::SimpleMath::Vector3 boradPos = { m_position.x, m_position.y + AREA_HALF_HEIGHT * 0.5f, m_position.z };
	m_numberBorad->SetPosition(boradPos);

	// 当たり判定の設定
	SetupCollider(pCM, x, z);
}



/**
 * @brief 更新処理
 *
 * @param cameraPos	カメラ位置
 * @param cameraUp	カメラ上ベクトル
 *
 * @return なし
 */
void CountArea::Update(float elapsedTime, const DirectX::SimpleMath::Vector3& cameraPos, const DirectX::SimpleMath::Vector3& cameraUp)
{
	// エフェクトの更新
	m_effect->Update(elapsedTime);

	// 数字UIのビルボードの作成
	m_numberBorad->CreateBillboard(cameraPos, cameraUp);
}



/**
 * @brief 描画処理
 *
 * @param context	描画用構造体
 * @param debugFont	デバッグ用フォント
 *
 * @return なし
 */
void CountArea::Draw(const RenderContext& context, DebugFont* debugFont)
{
	// ワールド行列の作成
	DirectX::SimpleMath::Vector3 size = m_collider.GetHalfLength();
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(size.x * 2.0f, size.y * 2.0f, size.z * 2.0f);
	world = scale * trans;

	// エフェクトの描画
	m_effect->Draw(context);

	// 数字の描画
	m_numberBorad->Draw(context);

	// 当たり判定のデバッグ描画
	//m_geometricPrimitive->Draw(world, context.view, context.proj, DirectX::Colors::Magenta, nullptr, true);

	// デバッグ情報の追加
	//debugFont->AddString(0, 170, Colors::Magenta, L" areaPos = %f,%f,%f", m_position.x, m_position.y, m_position.z);
	debugFont->AddString(0, 170, DirectX::Colors::Magenta, L"enter = %d", m_insideList.size());
	debugFont->AddString(110, 170, DirectX::Colors::Magenta, L"trigger = %d", m_isTrigger);
}



/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void CountArea::Finalize()
{
	m_geometricPrimitive.reset();

	if (m_numberBorad) m_numberBorad->Finalize();
	m_numberBorad.reset();
}



/**
 * @brief 当たり判定の初期設定
 *
 * @param pCM	当たり判定マネージャーのポインタ
 * @param x		X方向の長さ
 * @param z		Z方向の長さ
 *
 * @return なし
 */
void CountArea::SetupCollider(CollisionManager* pCM, float x, float z)
{
	// 当たり判定の設定
	m_collider.SetCenter(m_position);
	m_collider.SetHalfLength(DirectX::SimpleMath::Vector3(x, AREA_HALF_HEIGHT, z));
	m_collider.SetRotation(DirectX::SimpleMath::Quaternion::Identity);

	// 当たり判定マネージャーに登録
	CollisionManager::Desc desc{};
	desc.type = CollisionManager::Type::OBB;
	desc.layer = CollisionManager::Layer::Trigger;
	desc.obb = &m_collider;
	desc.position = nullptr;
	desc.velocity = nullptr;
	desc.isTrigger = true;
	desc.callback.onEnter =
		[this, pCM](uint32_t, uint32_t handle)
		{
			if (pCM->GetDesc(handle)->layer != CollisionManager::Layer::EnemyBody) return;
			EnterEnemy(pCM, handle);	// 敵をリストに追加
		};
	desc.callback.onExit =
		[this, pCM](uint32_t, uint32_t handle)
		{
			if (pCM->GetDesc(handle)->layer != CollisionManager::Layer::EnemyBody) return;
			ExitEnemy(pCM, handle);		// 敵をリストから除外
		};
	m_collisionHandle = pCM->Add(desc);
}



/**
 * @brief エリアに入った敵をリストに追加・条件判定
 *
 * @param pCM		当たり判定マネージャーのポインタ
 * @param handle	敵のハンドル
 *
 * @return なし
 */
void CountArea::EnterEnemy(CollisionManager* pCM, uint32_t handle)
{
#ifdef _DEBUG
	// デバッグ情報
	const auto* d = pCM->GetDesc(handle);
	auto epos = d->sphere->GetCenter();     // 敵の中心
	auto apos = m_collider.GetCenter();     // エリア中心
	auto ahl = m_collider.GetHalfLength();  // エリア半径
	char buf[256];
	sprintf_s(buf, "ENTER id=%u  enemy=(%.2f,%.2f,%.2f)  areaC=(%.2f,%.2f,%.2f)  areaHL=(%.2f,%.2f,%.2f)\n",
		d->userId, epos.x, epos.y, epos.z, apos.x, apos.y, apos.z, ahl.x, ahl.y, ahl.z);
	OutputDebugStringA(buf);
#endif

	// IDを取得
	uint32_t id = pCM->GetDesc(handle)->userId;
	if (id == 0) return;

	// リストに追加
	auto it = std::find(m_insideList.begin(), m_insideList.end(), id);
	if (it == m_insideList.end()) m_insideList.push_back(id);

	m_armed = true;

	// 条件を満たしているか(目標数以上入れる場合)
	if (m_mode == TriggerMode::ReachCount)
	{
		if (m_insideList.size() >= m_targetNum)	TriggerOn();
	}

	// 表示する数字の更新
	m_numberBorad->SetNumber(static_cast<int>(m_insideList.size()));
}



/**
 * @brief エリアを出た敵をリストから除外・条件判定
 *
 * @param pCM		当たり判定マネージャーのポインタ
 * @param handle	敵のハンドル
 *
 * @return なし
 */
void CountArea::ExitEnemy(CollisionManager* pCM, uint32_t handle)
{
#ifdef _DEBUG
	// デバッグ情報
	const auto* d = pCM->GetDesc(handle);
	char buf[256];
	sprintf_s(buf,
		"EXIT : handle=%u layer=%d userId=%u  enemyCount(before)=%zu\n",
		handle, (int)d->layer, d->userId, m_insideList.size());
	OutputDebugStringA(buf);
#endif

	// IDを取得
	uint32_t id = pCM->GetDesc(handle)->userId;

	// リストから削除
	m_insideList.erase(std::remove(m_insideList.begin(), m_insideList.end(), id), m_insideList.end());

	// 条件を満たしているか(内部の敵を全て出す場合)
	if (m_mode == TriggerMode::AllOut)
	{
		if (m_insideList.size() == 0 && m_armed) TriggerOn();
	}

	// 表示する数字の更新
	m_numberBorad->SetNumber(static_cast<int>(m_insideList.size()));
}



/**
 * @brief トリガーを起動
 *
 * @param なし
 *
 * @return なし
 */
void CountArea::TriggerOn()
{
	// トリガーをオン
	m_isTrigger = true;

	// 処理を実行
	m_operation();

	// 色を変更
	m_effect->SetColor(DirectX::Colors::Blue.v);
}
