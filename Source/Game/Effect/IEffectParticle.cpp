/**
 * @file   IEffectParticle.cpp
 *
 * @brief  パーティクルの基底クラスに関するソースファイル
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "IEffectParticle.h"


// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param なし
 */
IEffectParticle::IEffectParticle()
	: m_pDR(nullptr)
	, m_CBuffer(nullptr)
	, m_inputLayout(nullptr)
	, m_batch(nullptr)
	, m_states(nullptr)
	, m_texture()
	, m_vertices()
	, m_vertexShader(nullptr)
	, m_pixelShader(nullptr)
	, m_geometryShader(nullptr)
	, m_timer(0.0f)
	, m_isActive(false)
{

}


/**
 * @brief エフェクトの作成
 * 
 * @param pDR				デバイスリソースのポインタ
 * @param CBuffer			バッファのポインタ
 * @param inputLayout		インプットレイアウトのポインタ
 * @param batch				プリミティブバッチのポインタ
 * @param states			共通ステートのポインタ
 * @param texture			テクスチャのポインタ
 * @param vertexShader		頂点シェーダーのポインタ
 * @param pixelShader		ピクセルシェーダーのポインタ
 * @param geometryShader	ジオメトリシェーダーのポインタ
 * @param data				パーティクルのパラメータ
 *
 * @return なし
 */
void IEffectParticle::Create(
	DX::DeviceResources* pDR, 
	ID3D11Buffer* CBuffer, 
	ID3D11InputLayout* inputLayout, 
	DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>* batch,
	DirectX::CommonStates* states, 
	ID3D11ShaderResourceView* texture,
	ID3D11VertexShader* vertexShader,
	ID3D11PixelShader* pixelShader, 
	ID3D11GeometryShader* geometryShader,
	ParticleUtility::ParticleData data)
{
	{
		m_pDR = pDR;

		m_CBuffer = CBuffer;
		m_inputLayout = inputLayout;

		//画像の保存
		m_texture.push_back(texture);

		m_batch = batch;
		m_states = states;

		m_pixelShader = pixelShader;
		m_vertexShader = vertexShader;
		m_geometryShader = geometryShader;

		m_isActive = true;

		m_particleData = data;
	};
}


/**
 * @brief ビルボードの作成
 *
 * @param target	カメラのターゲットの座標
 * @param eye		カメラの座標
 * @param up		カメラの上ベクトル
 * @param forward	カメラの前ベクトル
 *
 * @return なし
 */
void IEffectParticle::CreateBillboard(
	DirectX::SimpleMath::Vector3 target,
	DirectX::SimpleMath::Vector3 eye, 
	DirectX::SimpleMath::Vector3 up,
	DirectX::SimpleMath::Vector3 forward)
{
	// 右ベクトルを計算する
	DirectX::SimpleMath::Vector3 camRight = up.Cross(forward);
	camRight.Normalize();

	// 上ベクトルを計算する
	DirectX::SimpleMath::Vector3 camUp = forward.Cross(camRight);
	
	// ビルボードの回転を作成する
	DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::Identity;
	rot.Forward(-forward);
	rot.Right(camRight);
	rot.Up(camUp);

	// 回転情報を設定する
	m_billboard = rot;

	// カメラ情報を設定する
	m_cameraPosition = eye;
	m_cameraTarget = target;
}



/**
 * @brief 描画処理
 *
 * @param view	ビュー行列
 * @param proj	射影行列
 *
 * @return なし
 */
void IEffectParticle::Draw(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();

	DirectX::SimpleMath::Vector3 cameraDir = m_cameraTarget - m_cameraPosition;
	cameraDir.Normalize();

	m_particleUtility.sort(
		[&](ParticleUtility lhs, ParticleUtility  rhs)
		{
			//	カメラ正面の距離でソート
			return cameraDir.Dot(lhs.GetPosition() - m_cameraPosition) > cameraDir.Dot(rhs.GetPosition() - m_cameraPosition);
		});

	// 頂点の作成
	m_vertices.clear();
	for (ParticleUtility& particle : m_particleUtility)
	{
		if (cameraDir.Dot(particle.GetPosition() - m_cameraPosition) < 0.0f) 	continue;

		DirectX::VertexPositionColorTexture vPCT;
		vPCT.position = DirectX::XMFLOAT3(particle.GetPosition());
		vPCT.color = DirectX::XMFLOAT4(particle.GetColor());
		vPCT.textureCoordinate = DirectX::XMFLOAT2(particle.GetScale().x, particle.GetScale().y);

		m_vertices.push_back(vPCT);
	}

	if (m_vertices.empty()) return;

	// ビルボードを反映
	m_world = DirectX::SimpleMath::Matrix::Identity;
	m_world *= m_billboard;

	ConstBuffer cbuff;

	cbuff.matView = view.Transpose();
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = m_world.Transpose();
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);

	context->UpdateSubresource(m_CBuffer, 0, NULL, &cbuff, 0, 0);

	ID3D11Buffer* cb[1] = { m_CBuffer };

	context->VSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { m_states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = m_states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	//	カリングはなし
	context->RSSetState(m_states->CullNone());

	//	シェーダをセットする
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);
	context->GSSetShader(m_geometryShader, nullptr, 0);

	//	Create関数で読み込んだ画像をピクセルシェーダに登録する
	for (int i = 0; i < m_texture.size(); i++)
	{
		//	for文で一気に設定する
		context->PSSetShaderResources(i, 1, &m_texture[i]);
	}

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout);

	//	板ポリゴンを描画
	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &m_vertices[0], m_vertices.size());
	m_batch->End();

	//	シェーダの登録を解除
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
}
