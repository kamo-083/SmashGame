//--------------------------------------------------------------------------------------
// File: Animation.h
//
// Simple animation playback system for CMO and SDKMESH for DirectX Tool Kit
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------
#pragma once

#include <DirectXMath.h>
#include <Model.h>

#include <memory>
#include <utility>
#include <vector>


namespace DX
{
    class AnimationSDKMESH
    {
    public:
        AnimationSDKMESH() noexcept;
        ~AnimationSDKMESH() = default;

        AnimationSDKMESH(AnimationSDKMESH&&) = default;
        AnimationSDKMESH& operator= (AnimationSDKMESH&&) = default;

        AnimationSDKMESH(AnimationSDKMESH const&) = delete;
        AnimationSDKMESH& operator= (AnimationSDKMESH const&) = delete;

        HRESULT Load(_In_z_ const wchar_t* fileName);

        void Release()
        {
            m_animTime = 0.0;
            m_animSize = 0;
            m_animData.reset();
            m_boneToTrack.clear();
            m_animBones.reset();
            m_frameDataPtrs.clear();    // ’Ç‰Á
        }

        bool Bind(const DirectX::Model& model);

        void Update(float delta);

        void Apply(
            const DirectX::Model& model,
            size_t nbones,
            _Out_writes_(nbones) DirectX::XMMATRIX* boneTransforms) const;

        void Reset();   // ’Ç‰Á

    private: 
        // cpp‚©‚çˆÚ“®
        static constexpr uint32_t SDKMESH_FILE_VERSION = 101;
        static constexpr uint32_t MAX_FRAME_NAME = 100;

        struct SDKANIMATION_DATA
        {
            DirectX::XMFLOAT3 Translation;
            DirectX::XMFLOAT4 Orientation;
            DirectX::XMFLOAT3 Scaling;
        };
        static_assert(sizeof(SDKANIMATION_DATA) == 40, "SDK Mesh structure size incorrect");

        struct SDKANIMATION_FRAME_DATA
        {
            char FrameName[MAX_FRAME_NAME];
            union
            {
                uint64_t DataOffset;
                SDKANIMATION_DATA* pAnimationData;
            };
        };
        static_assert(sizeof(SDKANIMATION_FRAME_DATA) == 112, "SDK Mesh structure size incorrect");

    private:
        double                              m_animTime;
        std::unique_ptr<uint8_t[]>          m_animData;
        size_t                              m_animSize;
        std::vector<uint32_t>               m_boneToTrack;
        DirectX::ModelBone::TransformArray  m_animBones;

        std::vector<const SDKANIMATION_DATA*> m_frameDataPtrs;  // ’Ç‰Á
    };

    class AnimationCMO
    {
    public:
        AnimationCMO() noexcept;
        ~AnimationCMO() = default;

        AnimationCMO(AnimationCMO&&) = default;
        AnimationCMO& operator= (AnimationCMO&&) = default;

        AnimationCMO(AnimationCMO const&) = delete;
        AnimationCMO& operator= (AnimationCMO const&) = delete;

        HRESULT Load(_In_z_ const wchar_t* fileName, size_t offset, _In_opt_z_ const wchar_t* clipName = nullptr);

        void Release()
        {
            m_animTime = m_startTime = m_endTime = 0.f;
            m_keys.clear();
            m_transforms.reset();
            m_animBones.reset();
        }

        void Bind(const DirectX::Model& model);

        void Update(float delta);

        void Apply(
            const DirectX::Model& model,
            size_t nbones,
            _Out_writes_(nbones) DirectX::XMMATRIX* boneTransforms) const;

    private:
        using Key = std::pair<uint32_t, float>;

        float                               m_animTime;
        float                               m_startTime;
        float                               m_endTime;
        std::vector<Key>                    m_keys;
        DirectX::ModelBone::TransformArray  m_transforms;
        DirectX::ModelBone::TransformArray  m_animBones;
    };
}