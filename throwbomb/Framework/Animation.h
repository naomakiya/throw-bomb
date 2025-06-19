#pragma once
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
       // 追加のアクセッサー
       // アニメーション時間を取得する
       double GetAnimTime() { return m_animTime; }
       // アニメーション時間を設定する
       void SetAnimTime(const double& animTime) { m_animTime = animTime; }
       // アニメーション開始時間を設定する
       void SetStartTime(const double& startTime) { m_animTime = startTime; }
       // アニメーション終了時間を取得する
       double GetEndTime() { return m_endTime; }
       // アニメーション終了時間を設定する
       void SetEndTime(const double& endTime) { m_endTime = endTime; }

       // AnimDataを取得する
       uint8_t* GetAnimData() { return m_animData.get(); }
       // 指定されたBoneToTrackを取得する
       uint32_t GetBoneToTrack(const int& index) { return m_boneToTrack[index]; }

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
      }

      bool Bind(const DirectX::Model& model);

      void Update(float delta);


      void Apply(
         const DirectX::Model& model,
         size_t nbones,
         _Out_writes_(nbones) DirectX::XMMATRIX* boneTransforms) const;

   private:
      double m_startTime;
      double m_endTime;
      double m_animTime;
      std::unique_ptr<uint8_t[]> m_animData;
      size_t  m_animSize;
      std::vector<uint32_t> m_boneToTrack;
      DirectX::ModelBone::TransformArray  m_animBones;
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

      float  m_animTime;
      float  m_startTime;
      float  m_endTime;
      std::vector<Key> m_keys;
      DirectX::ModelBone::TransformArray  m_transforms;
      DirectX::ModelBone::TransformArray  m_animBones;
   };
}
