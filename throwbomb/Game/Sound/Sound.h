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
	
	// FMOD�Ŏg�p����ϐ��i�|�C���^�j
	// FMOD�̃V�X�e��
	FMOD::System* m_system;	
	// SE�p�̉����f�[�^
	FMOD::Sound* m_soundSE;
	// BGM�p�̉����f�[�^
	FMOD::Sound* m_soundBGM;
	// SE���Đ�����`�����l��
	FMOD::Channel* m_seChannel;	
	// BGM���Đ�����`�����l��
	FMOD::Channel* m_bgmChannel;
	FMOD::Channel* m_channel;
	FMOD::Sound* m_currentBGMSound;

	float m_bgmVolume;
	float m_seVolume;

public:
	Sound();
	~Sound();
	void Initialize();
	void Update();
	void Finalize();

	// �����𓮓I�ɍĐ����郁�\�b�h
	//void PlaySounds(const std::string& filePath, bool loop );

	// ���ʂ�ݒ肷��
	void SetVolume(float volume);
	void StopBGM();
	void PlayBGM(const std::string& filePath, bool loop = true);
	void PlaySE(const std::string& filePath);
};
