/*
  @file BomExplosion.h
  @brief 爆発クラス
*/
#pragma once
#ifndef BOMEXPLOSION_DEFINED
#define BOMEXPLOSION_DEFINED
#include"Interface/IBomState.h"

class CommonResources;
class BomState;
class Wall;
class PlayerState;
class Dithering;
class Smork;
class Sound;

class BomExplosion :public IBomState
{
public:
    struct ExplosionObject {
        DirectX::SimpleMath::Vector3 offset;
        float scaleMultiplier;
        float startTime;                         // 開始時間
        float duration;                          // 表示し続ける時間
        bool active = true;
    };
	// コンストラクタ
    BomExplosion(BomState* bomState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
	//デストラクタ
	~BomExplosion();
	//初期化する
	void Initialize(CommonResources* resources);
	// 事前更新する
	void PreUpdate();
	//更新する
	void Update(const float& elapsedTime);
	// 事後更新する
	void PostUpdate();
	//描画する
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) ;
	//後処理を行う
	void Finalize();
private:
    //壁の破壊
    void CheckBreak();
    //シェイダーの読み取り
    void ShaderLoad(ID3D11Device* device);
    //バッファーの設定
    void CreateConstanBuffer(ID3D11Device* device);
private:
    // プレイヤー
    PlayerState* m_player;
    //煙パーティクル
    std::unique_ptr<Smork> m_smork;
    // 音
    std::unique_ptr<Sound> m_sound;
    // 壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // ピクセルシェーダ
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    //定数用バッファー
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    // 深度ステンシルステート
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    // 共通リソース
    CommonResources* m_commonResources;
    // BomState
    BomState* m_bomState;
    // 現在の状態
    IBomState* m_currentState;
    // 爆発球
    std::unique_ptr<DirectX::Model> m_explosionSphere;
    //ディザリング
    std::unique_ptr<Dithering> m_dithering;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    // バウンディングスフィア
    DirectX::BoundingSphere m_boundingSphere;
    // 複数爆破
    std::vector<ExplosionObject> m_explosionObjects;

    // Texture用メンバー（テクスチャーとスプライトバッチ）
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_explosionTexture;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

    // スケール
    float m_scale;
    // 爆発の持続時間
    float m_cnt;
    //経過時間
    float m_elapsedTime;
    // SEの音量を保存
    float m_seVolume;
};
#endif		// BOMEXPLOSION_DEFINED