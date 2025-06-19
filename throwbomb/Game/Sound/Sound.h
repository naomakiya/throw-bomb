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
	// シングルトン用の静的なインスタンス
	static std::unique_ptr<Sound> instance;
	static std::mutex mutex;
	
	// FMODで使用する変数（ポインタ）
	FMOD::System* m_system;	// FMODのシステム
	FMOD::Sound* m_soundSE;	// SE用の音声データ
	FMOD::Sound* m_soundBGM;	// BGM用の音声データ
	FMOD::Channel* m_seChannel;	// SEを再生するチャンネル
	FMOD::Channel* m_bgmChannel;// BGMを再生するチャンネル
	FMOD::Channel* m_channel;

	float m_bgmVolume;
	float m_seVolume;
public:
	// シングルトンインスタンスの取得
	static Sound& GetInstance();
	Sound();
	~Sound();
	void Initialize();
	void Update();
	void Finalize();

	// 音声を動的に再生するメソッド
	//void PlaySounds(const std::string& filePath, bool loop );

	// 音量を設定する
	void SetVolume(float volume);
	void PlayBGM(const std::string& filePath, bool loop = true);
	void PlaySE(const std::string& filePath);
};
