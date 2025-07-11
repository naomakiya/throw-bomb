/*
	@file	Sound.cpp
	@brief	�T�E���h�N���X
*/
#include"pch.h"
#include "Sound.h"
#include "Libraries/FMOD/inc/fmod.hpp"
#include "Libraries/FMOD/inc/fmod_errors.h"
#include <cassert>
#include <iostream>
#include <Framework/LoadJson.h>


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
Sound::Sound()
	:
	m_system{ nullptr },
	m_soundSE{ nullptr },
	m_soundBGM{ nullptr },
	m_seChannel{ nullptr },
	m_bgmChannel{ nullptr },
	m_channel{ nullptr },
	m_currentBGMSound{ nullptr },
	m_bgmVolume{0.0f},
	m_seVolume{0.0f}

{
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
Sound::~Sound()
{
	Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void Sound::Initialize()
{
	FMOD_RESULT result = FMOD::System_Create(&m_system);

	if (result != FMOD_OK) {
		std::cerr << "FMOD::System_Create failed: " << FMOD_ErrorString(result) << std::endl;
		return;
	}

	 result = m_system->init(32, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return;
	}

	LoadJson json("Resources/Json/Music.json");

	m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
	m_seVolume = json.GetJson()["SE"].value("Volume", 0.0f);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void Sound::Update() 
{
	if (m_system) {
		m_system->update();
	}
}


//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void Sound::Finalize() 
{
	/*m_channel->stop();
	FMOD_RESULT result = m_system->close();
	result = m_system->release();
	m_system = nullptr;*/
	
	if (m_currentBGMSound) {
		m_currentBGMSound->release();
		m_currentBGMSound = nullptr;
	}
	if (m_system) {
		m_system->close();
		m_system->release();
		m_system = nullptr;
	}
}

//void Sound::PlaySounds(const std::string& filePath, bool loop) 
//{
//	assert(m_system);
//
//	FMOD::Sound* sound = nullptr;
//	//FMOD::Channel* channel = nullptr;
//
//	// ���������[�h
//	FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
//
//	FMOD_RESULT result = m_system->createSound(filePath.c_str(), mode, nullptr, &sound);
//	if (result != FMOD_OK) 
//	{
//		std::cerr << "Failed to load sound: " << filePath << " (" << FMOD_ErrorString(result) << ")" << std::endl;
//		return;
//	}
//
//	// �������Đ�
//	result = m_system->playSound(sound, nullptr, false, &m_channel);
//	if (result != FMOD_OK) 
//	{
//		std::cerr << "Failed to play sound: " << filePath << " (" << FMOD_ErrorString(result) << ")" << std::endl;
//	}
//
//	m_channel->setVolume(m_bgmVolume);
//	
//}

void Sound::PlayBGM(const std::string& filePath, bool loop)
{
	if (m_currentBGMSound) {
		m_currentBGMSound->release();  // �ȑO�̉������
		m_currentBGMSound = nullptr;
	}

	FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
	FMOD_RESULT result = m_system->createSound(filePath.c_str(), mode, nullptr, &m_currentBGMSound);
	if (result != FMOD_OK) return;

	if (m_bgmChannel) m_bgmChannel->stop();
	m_system->playSound(m_currentBGMSound, nullptr, false, &m_bgmChannel);
	m_bgmChannel->setVolume(m_bgmVolume);
}

void Sound::PlaySE(const std::string& filePath)
{
	assert(m_system);

	FMOD::Sound* sound = nullptr;

	FMOD_RESULT result = m_system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
	if (result != FMOD_OK)
	{
		std::cerr << "SE load error: " << filePath << " (" << FMOD_ErrorString(result) << ")" << std::endl;
		return;
	}

	FMOD::Channel* seChannel = nullptr;
	result = m_system->playSound(sound, nullptr, false, &seChannel);
	if (result != FMOD_OK)
	{
		std::cerr << "SE play error: " << filePath << " (" << FMOD_ErrorString(result) << ")" << std::endl;
	}

	seChannel->setVolume(m_seVolume);
}

void Sound::SetVolume(float volume) 
{
	if (m_bgmChannel) {
		m_bgmChannel->setVolume(volume);
	}
	
}

void Sound::StopBGM()
{
	m_bgmChannel->stop();
}