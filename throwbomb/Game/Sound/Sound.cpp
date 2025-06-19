/*
	@file	Sound.cpp
	@brief	サウンドクラス
*/
#include"pch.h"
#include "Sound.h"
#include "Libraries/FMOD/inc/fmod.hpp"
#include "Libraries/FMOD/inc/fmod_errors.h"
#include <cassert>
#include <iostream>
#include <Framework/LoadJson.h>

// 静的メンバの初期化
std::unique_ptr<Sound> Sound::instance = nullptr;
std::mutex Sound::mutex;

// シングルトンインスタンスの取得
Sound& Sound::GetInstance() 
{
	std::lock_guard<std::mutex> lock(mutex);
	if (!instance) 
	{
		instance.reset(new Sound());
	}
	return *instance;
}

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Sound::Sound()
	:
	m_system{ nullptr },
	m_soundSE{ nullptr },
	m_soundBGM{ nullptr },
	m_seChannel{ nullptr },
	m_bgmChannel{ nullptr },
	m_channel{nullptr},
	m_bgmVolume{0.0f},
	m_seVolume{0.0f}

{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Sound::~Sound()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
// 初期化処理
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
// 更新する
//---------------------------------------------------------
void Sound::Update() 
{
	if (m_system) {
		m_system->update();
	}
}


//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void Sound::Finalize() 
{
	m_channel->stop();
	FMOD_RESULT result = m_system->close();
	result = m_system->release();
	m_system = nullptr;
	
}

//void Sound::PlaySounds(const std::string& filePath, bool loop) 
//{
//	assert(m_system);
//
//	FMOD::Sound* sound = nullptr;
//	//FMOD::Channel* channel = nullptr;
//
//	// 音声をロード
//	FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
//
//	FMOD_RESULT result = m_system->createSound(filePath.c_str(), mode, nullptr, &sound);
//	if (result != FMOD_OK) 
//	{
//		std::cerr << "Failed to load sound: " << filePath << " (" << FMOD_ErrorString(result) << ")" << std::endl;
//		return;
//	}
//
//	// 音声を再生
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
	assert(m_system);

	FMOD::Sound* sound = nullptr;

	FMOD_MODE mode = loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;

	FMOD_RESULT result = m_system->createSound(filePath.c_str(), mode, nullptr, &sound);
	if (result != FMOD_OK)
	{
		std::cerr << "BGM load error: " << filePath << " (" << FMOD_ErrorString(result) << ")" << std::endl;
		return;
	}

	// 既存のBGM止める（必要なら）
	if (m_bgmChannel) m_bgmChannel->stop();

	result = m_system->playSound(sound, nullptr, false, &m_bgmChannel);
	if (result != FMOD_OK)
	{
		std::cerr << "BGM play error: " << filePath << " (" << FMOD_ErrorString(result) << ")" << std::endl;
	}

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