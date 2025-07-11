/*
	@file	Sound.h
	@brief	音クラス
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
	
	// FMODで使用する変数（ポインタ）
	// FMODのシステム
	FMOD::System* m_system;	
	// SE用の音声データ
	FMOD::Sound* m_soundSE;
	// BGM用の音声データ
	FMOD::Sound* m_soundBGM;
	// SEを再生するチャンネル
	FMOD::Channel* m_seChannel;	
	// BGMを再生するチャンネル
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

	// 音声を動的に再生するメソッド
	//void PlaySounds(const std::string& filePath, bool loop );

	// 音量を設定する
	void SetVolume(float volume);
	void StopBGM();
	void PlayBGM(const std::string& filePath, bool loop = true);
	void PlaySE(const std::string& filePath);
};
