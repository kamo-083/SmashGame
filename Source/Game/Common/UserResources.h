//--------------------------------------------------------------------------------------
// File: UserResources.h
//
// シーンへ渡すユーザーリソースクラス（サンプル）
//
// Date: 2025.7.31
//--------------------------------------------------------------------------------------
#pragma once

#include "pch.h"
#include "StepTimer.h"
#include "DeviceResources.h"
#include "ImaseLib/DebugFont.h"
#include "ResourceManager.h"
#include "Source/Game/Effect/EffectManager.h"

// 各シーンに渡す共通リソースを記述してください
class UserResources
{
private:

	// ステップタイマー
	DX::StepTimer* m_timer;

	// デバイスリソース
	DX::DeviceResources* m_deviceResources;

	// キーボードステートトラッカー
	DirectX::Keyboard::KeyboardStateTracker* m_keyboardTracker;

	// マウスステートトラッカー
	DirectX::Mouse::ButtonStateTracker* m_mouseTracker;

	// デバッグ用文字列表示へのポインタ
	Imase::DebugFont* m_debugFont;

	// 共通ステート
	DirectX::CommonStates* m_states;

	// スプライトバッチ
	DirectX::SpriteBatch* m_spriteBatch;

	// リソースマネージャー
	ResourceManager* m_resourceManager;


public:

	// コンストラクタ
	UserResources()
		: m_timer(nullptr)
		, m_deviceResources(nullptr)
		, m_keyboardTracker(nullptr)
		, m_mouseTracker(nullptr)
		, m_debugFont(nullptr)
		, m_states(nullptr)
		, m_spriteBatch(nullptr)
		, m_resourceManager(nullptr)
	{
	}

	// デストラクタ
	~UserResources()
	{
		m_timer = nullptr;
		m_deviceResources = nullptr;
		m_keyboardTracker = nullptr;
		m_mouseTracker = nullptr;
		m_debugFont = nullptr;
		m_states = nullptr;
		m_spriteBatch = nullptr;
		m_resourceManager = nullptr;
	}

	// ------------------------------------------ //
	// ステップタイマー	
	// ------------------------------------------ //

	// ステップタイマーを設定する関数
	void SetStepTimerStates(DX::StepTimer* timer) { m_timer = timer; }

	// ステップタイマーを取得する関数
	DX::StepTimer* GetStepTimer() { return m_timer; }

	// ------------------------------------------ //
	// デバイスリソース	
	// ------------------------------------------ //

	// デバイスリソースを設定する関数
	void SetDeviceResources(DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }

	// デバイスリソースを取得する関数
	DX::DeviceResources* GetDeviceResources() { return m_deviceResources; }
	
	// ------------------------------------------ //
	// キーボードステートトラッカー	
	// ------------------------------------------ //

	// キーボードステートトラッカーを設定する関数
	void SetKeyboardTracker(DirectX::Keyboard::KeyboardStateTracker* keyboardTracker) { m_keyboardTracker = keyboardTracker; }

	// キーボードステートトラッカーを取得する関数
	DirectX::Keyboard::KeyboardStateTracker* GetKeyboardTracker() { return m_keyboardTracker; }

	// ------------------------------------------ //
	// デバッグフォント	
	// ------------------------------------------ //

	// デバッグフォントを設定する関数
	void SetDebugFont(Imase::DebugFont* debugFont) { m_debugFont = debugFont; }

	// デバッグフォントを取得する関数
	Imase::DebugFont* GetDebugFont() { return m_debugFont; }

	// ------------------------------------------ //
	// スプライトバッチ
	// ------------------------------------------ //
		
	// スプライトバッチを設定する関数
	void SetSpriteBatch(DirectX::SpriteBatch* spriteBatch) { m_spriteBatch = spriteBatch; }

	// スプライトバッチを取得する関数
	DirectX::SpriteBatch* GetSpriteBatch() { return m_spriteBatch; }

	// ------------------------------------------ //
	// リソースマネージャー
	// ------------------------------------------ //
		
	// リソースマネージャーを設定する関数
	void SetResourceManager(ResourceManager* resourceManager) { m_resourceManager = resourceManager; }

	// リソースマネージャーを取得する関数
	ResourceManager* GetResourceManager() { return m_resourceManager; }
	
};

