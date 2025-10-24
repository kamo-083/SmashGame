//
// Game.h
//

#pragma once

#include "Source/Game/Common/DeviceResources.h"
#include "Source/Game/Common/StepTimer.h"
#include "Source/Game/Common/SceneManager.h"
#include "Source/Game/Common/ResourceManager.h"
#include "Source/Game/Common/AudioManager.h"
#include "Source/Game/Common/ShaderManager.h"
#include "Source/Game/Common/Scene.h"
#include "Source/Game/Common/UserResources.h"
#include "Source/Debug/DebugFont.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

    void Shutdown();

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    //共通ステートへのポインタ
    std::unique_ptr<DirectX::CommonStates> m_states;

    //スプライトバッチ
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

    // 射影行列
    DirectX::SimpleMath::Matrix m_proj;

    // シーンマネージャー
    std::unique_ptr<SceneManager> m_sceneManager;

    // リソースマネージャー
    std::unique_ptr<ResourceManager> m_resourceManager;
    
    // オーディオマネージャー
    std::unique_ptr<AudioManager> m_audioManager;

    // シェーダーマネージャー
    std::unique_ptr<ShaderManager> m_shaderManager;

    // ユーザーリソース
    std::unique_ptr<UserResources> m_userResources;

    // デバッグフォント
    std::unique_ptr <DebugFont> m_debugFont;

    // キーボードトラッカー
    std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_kbTracker;

};
