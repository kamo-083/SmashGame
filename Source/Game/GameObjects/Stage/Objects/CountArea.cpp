/**
 * @file   CountArea.cpp
 *
 * @brief  エリアに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "CountArea.h"


// インプットレイアウトの設定
const std::vector<D3D11_INPUT_ELEMENT_DESC> CountArea::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param pUR	ユーザーリソースのポインタ
 */
CountArea::CountArea(UserResources* pUR)
	: m_mode(TriggerMode::ReachCount)
	, m_targetNum(0)
	, m_isTrigger(false)
	, m_armed(false)
	, m_insideList()
	, m_collisionHandle(0)
	, m_timer(0.0f)
{
	DX::DeviceResources* dr = pUR->GetDeviceResources();
	ResourceManager*	 rm = pUR->GetResourceManager();

	m_geometricPrimitive = DirectX::GeometricPrimitive::CreateBox(dr->GetD3DDeviceContext(), { 1.0f, 1.0f, 1.0f }, true);

	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(dr->GetD3DDeviceContext());

	m_numberBorad = std::make_unique<NumberRenderer3D>(
		NUMBER_SIZE,
		rm->RequestPNG("number", "Text/number_48.png"),
		1,
		dr,
		0.5f
	);

	// シェーダーの読み込み
	LoadShaders(pUR->GetShaderManager(), dr->GetD3DDevice());
}



/**
 * @brief デストラクタ
 */
CountArea::~CountArea()
{
	m_vs = nullptr;
	m_ps = nullptr;
	m_gs = nullptr;

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
	DirectX::SimpleMath::Vector3 position, float x, float z,
	std::function<void()> operation, TriggerMode mode, int targetNum)
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

	// タイマーをリセット
	m_timer = 0.0f;

	// 色を設定
	m_color = DirectX::Colors::Red;

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
void CountArea::Update(float elapsedTime, DirectX::SimpleMath::Vector3 cameraPos, DirectX::SimpleMath::Vector3 cameraUp)
{
	// 経過時間の加算
	m_timer += elapsedTime;

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
void CountArea::Draw(RenderContext& context, DebugFont* debugFont)
{
	// ワールド行列の作成
	DirectX::SimpleMath::Vector3 size = m_collider.GetHalfLength();
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(size.x * 2.0f, size.y * 2.0f, size.z * 2.0f);
	world = scale * trans;

	// 数字の描画
	m_numberBorad->Draw(context);

	// 囲いの描画
	DrawArea(context, world, size);

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
 * @brief シェーダーの読み込み
 *
 * @param shaderManager シェーダーマネージャー
 * @param device デバイスのポインタ
 *
 * @return なし
 */
void CountArea::LoadShaders(ShaderManager* shaderManager, ID3D11Device* device)
{
	// 頂点シェーダー
	shaderManager->CreateVS("areaVS", "Shaders/Area/AreaVS.cso", INPUT_LAYOUT);
	m_vs = shaderManager->GetVS("areaVS");

	// ピクセルシェーダー
	shaderManager->CreatePS("areaPS", "Shaders/Area/AreaPS.cso");
	m_ps = shaderManager->GetPS("areaPS");

	// ジオメトリシェーダー
	shaderManager->CreateGS("areaGS", "Shaders/Area/AreaGS.cso");
	m_gs = shaderManager->GetGS("areaGS");

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}



/**
 * @brief エリアの描画
 *
 * @param context	描画用構造体
 * @param world		ワールド行列
 * @param areaSize	エリアの大きさ
 *
 * @return なし
 */
void CountArea::DrawArea(
	RenderContext& context,
	DirectX::SimpleMath::Matrix& world,
	DirectX::SimpleMath::Vector3& areaSize)
{
	ConstBuffer cbuff;
	cbuff.matView = context.view.Transpose();
	cbuff.matProj = context.proj.Transpose();
	cbuff.matWorld = world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.Height = AREA_HALF_HEIGHT * 2.0f;
	cbuff.Time = m_timer;
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };

	context.deviceContext->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	context.deviceContext->VSSetConstantBuffers(0, 1, cb);
	context.deviceContext->PSSetConstantBuffers(0, 1, cb);
	context.deviceContext->GSSetConstantBuffers(0, 1, cb);

	// 半透明描画指定
	ID3D11BlendState* blendstate = context.states->NonPremultiplied();

	// 透明判定処理
	context.deviceContext->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	// 深度バッファは読み取り専用
	context.deviceContext->OMSetDepthStencilState(context.states->DepthRead(), 0);

	// カリングはなし
	context.deviceContext->RSSetState(context.states->CullNone());

	// シェーダをセットする
	context.deviceContext->VSSetShader(m_vs->vs.Get(), nullptr, 0);
	context.deviceContext->PSSetShader(m_ps->ps.Get(), nullptr, 0);
	context.deviceContext->GSSetShader(m_gs->gs.Get(), nullptr, 0);

	// インプットレイアウトの登録
	context.deviceContext->IASetInputLayout(m_vs->inputLayout.Get());

	float vertexes[4];
	vertexes[0] = m_position.z + areaSize.z;	// 奥
	vertexes[1] = m_position.x + areaSize.x;	// 右
	vertexes[2] = m_position.z - areaSize.z;	// 手前
	vertexes[3] = m_position.x - areaSize.x;	// 左

	DirectX::VertexPositionColor v[2];
	float yPos = m_position.y - AREA_HALF_HEIGHT;

	// 板ポリゴンを描画
	m_batch->Begin();

	// 奥
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[3], yPos, vertexes[0]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[1], yPos, vertexes[0]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	// 右
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[1], yPos, vertexes[2]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[1], yPos, vertexes[0]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	// 左
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[3], yPos, vertexes[2]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[3], yPos, vertexes[0]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	// 手前
	v[0].position = DirectX::SimpleMath::Vector3(vertexes[3], yPos, vertexes[2]);
	v[0].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	v[1].position = DirectX::SimpleMath::Vector3(vertexes[1], yPos, vertexes[2]);
	v[1].color = static_cast<DirectX::SimpleMath::Vector4>(m_color);
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, v, 2);

	m_batch->End();

	// シェーダの登録を解除
	context.deviceContext->VSSetShader(nullptr, nullptr, 0);
	context.deviceContext->PSSetShader(nullptr, nullptr, 0);
	context.deviceContext->GSSetShader(nullptr, nullptr, 0);
}

void CountArea::TriggerOn()
{
	// トリガーをオン
	m_isTrigger = true;

	// 処理を実行
	m_operation();

	// 色を変更
	m_color = DirectX::Colors::Blue;
}
