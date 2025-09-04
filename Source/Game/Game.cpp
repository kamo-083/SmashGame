//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Source/Game/Common/RenderContext.h"
#include "Source/Game/Scenes/TestScene.h"
#include "Source/Game/Scenes/TitleScene.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // スプライトバッチの作成
    m_spriteBatch = std::make_unique<SpriteBatch>(context);
    
    // 各シーンの作成
    m_sceneManager->Register("TestScene", std::make_unique<TestScene>(m_sceneManager.get(), m_userResources.get()));
    m_sceneManager->Register("TitleScene", std::make_unique<TitleScene>(m_sceneManager.get(), m_userResources.get()));

    // 開始シーンの設定
    //m_sceneManager->SetStartScene("TitleScene");
    m_sceneManager->SetStartScene("TestScene");
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

    // シーンマネージャの更新
    m_sceneManager->Update(elapsedTime);

    // Escでゲームを閉じる
    auto kb = Keyboard::Get().GetState();
    if (kb.Escape)
    {
        ExitGame();
    }
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    RenderContext renderContext
    {
        SimpleMath::Matrix::Identity,
        m_proj,
        m_deviceResources->GetD3DDeviceContext(),
        m_states.get(),
        m_spriteBatch.get()
    };

    // シーンマネージャの描画
    m_sceneManager->Render(renderContext);

    // デバッグフォントの描画
    m_debugFont->Render(m_states.get());

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}


// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1280;
    height =720;
}

void Game::Shutdown()
{
    m_sceneManager.reset();
    m_resourceManager.reset();

    m_spriteBatch.reset();
    m_states.reset();
    m_debugFont.reset();

    m_userResources.reset();

    auto context = m_deviceResources->GetD3DDeviceContext();
    context->OMSetRenderTargets(0, nullptr, nullptr);
    context->ClearState();
    context->Flush();

    m_deviceResources.reset();
}

#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).

    // 共通ステートの作成
    m_states = std::make_unique<CommonStates>(device);

    // リソースマネージャーの作成
    m_resourceManager = std::make_unique<ResourceManager>(device);

    // デバッグフォントの作成
    m_debugFont = std::make_unique<Imase::DebugFont>(device, context, L"Resources/Font/SegoeUI_18.spritefont");

    // ユーザーリソースの作成
    m_userResources = std::make_unique<UserResources>();

    // ユーザーリソースの設定
    m_userResources->SetStepTimerStates(&m_timer);
    m_userResources->SetDeviceResources(m_deviceResources.get());
    m_userResources->SetDebugFont(m_debugFont.get());
    m_userResources->SetResourceManager(m_resourceManager.get());

     // シーンマネージャの作成
    m_sceneManager = std::make_unique<SceneManager>(m_userResources.get());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    // 画面サイズの取得
    RECT rect = m_deviceResources->GetOutputSize();

    // 射影行列の作成
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        XMConvertToRadians(45.0f)
        , static_cast<float>(rect.right) / static_cast<float>(rect.bottom)
        , 0.1f, 100.0f);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
