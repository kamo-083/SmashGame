/**
 * @file   ModelAnimator.h
 *
 * @brief  ModelAnimatorに関するヘッダファイル
 *
 * @author 清水まこと
 */

 // 多重インクルードの防止 =====================================================
#pragma once



// ヘッダファイルの読み込み ===================================================
#include"Source/Game/Common/Animation.h"
#include"Source/Game/Common/RenderContext.h"


// クラスの定義 ===============================================================
/**
 * @brief ModelAnimator
 */
class ModelAnimator
{
	// クラス定数の宣言 -------------------------------------------------
private:



	// データメンバの宣言 -----------------------------------------------
private:
	// モデル
	DirectX::Model* m_model;

	// アニメーション
	DX::AnimationSDKMESH* m_animation;

	// ボーン配列
	DirectX::ModelBone::TransformArray m_drawBones;

	// 経過時間
	float m_animElapsedTime;

	// アニメーション終了時間
	float m_animEndTime;

	// ループさせるか
	bool m_loop;


	// メンバ関数の宣言 -------------------------------------------------
	// コンストラクタ/デストラクタ
public:
	// コンストラクタ
	ModelAnimator(DirectX::Model* model, DX::AnimationSDKMESH* animation);

	// デストラクタ
	~ModelAnimator();


// 操作
public:
	// 初期化処理
	void Initialize(float endTime = FLT_MAX, bool loop = true);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(RenderContext context, const DirectX::SimpleMath::Matrix& world);

	// 終了処理
	void Finalize();

// 取得/設定
public:

// 内部実装
private:

};
