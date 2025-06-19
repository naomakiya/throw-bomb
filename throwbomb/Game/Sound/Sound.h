/*
	@file	Sound.h
	@brief	���N���X
*/
#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

class Sound
{
private:
	// �V���O���g���p�̐ÓI�ȃC���X�^���X
	static std::unique_ptr<Sound> instance;
	static std::mutex mutex;
	
	// FMOD�Ŏg�p����ϐ��i�|�C���^�j
	FMOD::System* m_system;	// FMOD�̃V�X�e��
	FMOD::Sound* m_soundSE;	// SE�p�̉����f�[�^
	FMOD::Sound* m_soundBGM;	// BGM�p�̉����f�[�^
	FMOD::Channel* m_seChannel;	// SE���Đ�����`�����l��
	FMOD::Channel* m_bgmChannel;// BGM���Đ�����`�����l��
	FMOD::Channel* m_channel;

	float m_bgmVolume;
	float m_seVolume;
public:
	// �V���O���g���C���X�^���X�̎擾
	static Sound& GetInstance();
	Sound();
	~Sound();
	void Initialize();
	void Update();
	void Finalize();

	// �����𓮓I�ɍĐ����郁�\�b�h
	//void PlaySounds(const std::string& filePath, bool loop );

	// ���ʂ�ݒ肷��
	void SetVolume(float volume);
	void PlayBGM(const std::string& filePath, bool loop = true);
	void PlaySE(const std::string& filePath);
};
