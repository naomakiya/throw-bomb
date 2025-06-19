/*
   @file Interface/IBomState.h
   @brief ボムのインターフェイスクラス
*/
#pragma once
#ifndef IBOMSTATE_DEFINED
#define IBOMSTATE_DEFINED
// 前方宣言
class CommonResources;

class IBomState
{
public:
    // 爆弾の状態
    enum BomtPresent : short { Exist, MOVEMENT,STAY ,EXPLOSION };
public:
    virtual ~IBomState() = default;
    //初期化する
    virtual void Initialize(CommonResources* resources) = 0;
    //事前更新
    virtual void PreUpdate() = 0;
    //更新する
    virtual void Update(const float& elapsedTime) = 0;
    //事後更新
    virtual void PostUpdate() = 0;
    //描画する
    virtual void Render(const DirectX::SimpleMath::Matrix& view, 
                        const DirectX::SimpleMath::Matrix& projection) = 0;
    //後処理を行う 
    virtual void Finalize() = 0;

   
};

#endif