/*
  @file BomExplosion.h
  @brief �����N���X
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
        float startTime;                         // �J�n����
        float duration;                          // �\���������鎞��
        bool active = true;
    };
	// �R���X�g���N�^
    BomExplosion(BomState* bomState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
	//�f�X�g���N�^
	~BomExplosion();
	//����������
	void Initialize(CommonResources* resources);
	// ���O�X�V����
	void PreUpdate();
	//�X�V����
	void Update(const float& elapsedTime);
	// ����X�V����
	void PostUpdate();
	//�`�悷��
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) ;
	//�㏈�����s��
	void Finalize();
private:
    //�ǂ̔j��
    void CheckBreak();
    //�V�F�C�_�[�̓ǂݎ��
    void ShaderLoad(ID3D11Device* device);
    //�o�b�t�@�[�̐ݒ�
    void CreateConstanBuffer(ID3D11Device* device);
private:
    // �v���C���[
    PlayerState* m_player;
    //���p�[�e�B�N��
    std::unique_ptr<Smork> m_smork;
    // ��
    std::unique_ptr<Sound> m_sound;
    // ��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // �s�N�Z���V�F�[�_
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    //�萔�p�o�b�t�@�[
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    // �[�x�X�e���V���X�e�[�g
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // BomState
    BomState* m_bomState;
    // ���݂̏��
    IBomState* m_currentState;
    // ������
    std::unique_ptr<DirectX::Model> m_explosionSphere;
    //�f�B�U�����O
    std::unique_ptr<Dithering> m_dithering;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �o�E���f�B���O�X�t�B�A
    DirectX::BoundingSphere m_boundingSphere;
    // �������j
    std::vector<ExplosionObject> m_explosionObjects;

    // Texture�p�����o�[�i�e�N�X�`���[�ƃX�v���C�g�o�b�`�j
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_explosionTexture;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

    // �X�P�[��
    float m_scale;
    // �����̎�������
    float m_cnt;
    //�o�ߎ���
    float m_elapsedTime;
    // SE�̉��ʂ�ۑ�
    float m_seVolume;
};
#endif		// BOMEXPLOSION_DEFINED