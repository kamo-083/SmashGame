/**
 * @file   BillboardSprite.cpp
 *
 * @brief  BillboardSpriteに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "BillboardSprite.h"

// 頂点位置・各頂点のUV座標
const DirectX::VertexPositionTexture BillboardSprite::VERTECES[4] =
{
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(0.5f,  0.5f, 0.0f),  DirectX::SimpleMath::Vector2(1.0f, 0.0f)),   // 0:左上
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(0.5f, -0.5f, 0.0f),  DirectX::SimpleMath::Vector2(1.0f, 1.0f)),   // 1:右上
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(-0.5f, -0.5f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 1.0f)),   // 3:右下
	DirectX::VertexPositionTexture(DirectX::SimpleMath::Vector3(-0.5f,  0.5f, 0.0f), DirectX::SimpleMath::Vector2(0.0f, 0.0f)),   // 2:左下
};

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
BillboardSprite::BillboardSprite(DX::DeviceResources* pDR)
	:
	TweenObject(),
	m_isBillboard(false)
{
	// ポインタを取得
	ID3D11Device* device = pDR->GetD3DDevice();
	ID3D11DeviceContext* context = pDR->GetD3DDeviceContext();

	// エフェクトの作成
	m_batchEffect = std::make_unique<DirectX::AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);

	// 入力レイアウトの生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		DirectX::VertexPositionTexture::InputElements,
		DirectX::VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf()
	);

	// プリミティブバッチの作成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(context);
}

/**
 * @brief デストラクタ
 */
BillboardSprite::~BillboardSprite()
{

}

/**
 * @brief 初期化処理
 *
 * @param texture	テクスチャのポインタ
 * @param data		トゥイーン情報
 * @param size		テクスチャのサイズ
 *
 * @return なし
 */
void BillboardSprite::Initialize(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture,
	const Tween3D::TweenData& data,
	const DirectX::SimpleMath::Vector2& size)
{
	// テクスチャ・アニメーションの設定
	m_texture = texture;
	m_params = data.start;
	m_texSize = size;

	m_tween = std::make_unique<Tween3D>(data);
	m_tween->Play();

	m_isBillboard = false;
}

/**
 * @brief 更新処理
 *
 * @param elapsedTime 経過時間
 *
 * @return なし
 */
void BillboardSprite::Update(float elapsedTime)
{
	// パラメータの更新
	m_tween->Update(elapsedTime, m_params);
}

/**
 * @brief 描画処理
 *
 * @param context 描画に必要な情報をまとめた構造体
 *
 * @return なし
 */
void BillboardSprite::Draw(const RenderContext& context)
{
	// 頂点情報
	DirectX::VertexPositionTexture vertex[4]{};
	for (int i = 0; i < 4; i++)
	{
		vertex[i] = VERTECES[i];
	}

	// テクスチャサンプラーの設定
	ID3D11SamplerState* samplers[1] = { context.states->LinearClamp() };
	context.deviceContext->PSSetSamplers(0, 1, samplers);

	//	半透明描画指定
	ID3D11BlendState* blendstate = context.states->AlphaBlend();
	//	透明判定処理
	context.deviceContext->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	// 深度テストの無効化
	context.deviceContext->OMSetDepthStencilState(context.states->DepthRead(), 0);

	// カリングの設定
	context.deviceContext->RSSetState(context.states->CullNone());

	// ワールド行列の作成
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix pos = DirectX::SimpleMath::Matrix::CreateTranslation(m_params.pos);
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_params.rotation);
	DirectX::SimpleMath::Matrix scale = DirectX::SimpleMath::Matrix::CreateScale(m_params.scale);
	world = scale * rot;

	// ビルボードを反映
	if (m_isBillboard)	world *= m_billboard;
	else				world *= pos;

	// 不透明のみ描画する設定
	m_batchEffect->SetTexture(m_texture.Get());
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_batchEffect->SetReferenceAlpha(0);
	m_batchEffect->SetWorld(world);
	m_batchEffect->SetView(context.view);
	m_batchEffect->SetProjection(context.proj);
	m_batchEffect->Apply(context.deviceContext);
	context.deviceContext->IASetInputLayout(m_inputLayout.Get());

	// ポリゴンを描画 
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_primitiveBatch->End();

	// 深度テストの設定をもとに戻す
	context.deviceContext->OMSetDepthStencilState(context.states->DepthDefault(), 0);
}

/**
 * @brief 終了処理
 *
 * @param なし
 *
 * @return なし
 */
void BillboardSprite::Finalize()
{

}

/**
 * @brief ビルボードの作成
 *
 * @param eye		カメラの位置
 * @param up		カメラの上方向
 * @param forward	カメラの前方向
 *
 * @return なし
 */
void BillboardSprite::CreateBillboard(
	const DirectX::SimpleMath::Vector3& eye,
	const DirectX::SimpleMath::Vector3& up)
{
	// ビルボード状態に設定
	m_isBillboard = true;

	// 行列を作成
	m_billboard = DirectX::SimpleMath::Matrix::CreateBillboard(m_params.pos, eye, up);

	//Y軸を180度回転させる
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::Identity;
	rotY._11 = -1.0f;
	rotY._33 = -1.0f;

	//ビルボードを回転させる
	m_billboard = rotY * m_billboard;
}

/**
 * @brief トゥイーンのリセット
 *
 * @param play 再生するか
 *
 * @return なし
 */
void BillboardSprite::TweenReset(bool play)
{
	// パラメータ・アニメーションの初期化
	m_params = m_tween->GetStartParams();
	m_tween->ResetTime();
	m_tween->ResetPlayed();

	// すぐに再生するか
	if (play) m_tween->Play();
	else	  m_tween->End();
}

/**
 * @brief パラメータの設定
 *
 * @param start	開始時のパラメータ
 * @param delta	パラメータの変化量
 *
 * @return なし
 */
void BillboardSprite::SetParam(const Tween3D::TweenParams& start, const Tween3D::TweenParams& delta)
{
	// 現在のパラメータを更新
	m_params = start;

	// アニメーションにパラメータを設定
	Tween3D::TweenData data = m_tween->GetTweenData();
	data.start = start;
	data.delta = delta;
	m_tween->SetTweenData(data);
}

/**
 * @brief イージングの種類を設定
 *
 * @param type イージングの種類
 *
 * @return なし
 */
void BillboardSprite::SetEaseType(const Easing::EaseType type)
{
	Tween3D::TweenData data = m_tween->GetTweenData();
	data.ease = type;
	m_tween->SetTweenData(data);
}
