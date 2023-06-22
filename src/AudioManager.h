#pragma once
#include <map>
#include <vector>

#include "Log.h"
#include "SoundStream.h"

#include "array"

class SoundStream;
class SoundEffect;

class AudioManager
{
public:


	enum class MusicOptions
	{
		None,
		MaximumPower,
		FinalTakeOff,
		RedOut,
		SuperStripe,
		AfterBurner,
		City202,
	};


	static void Initialize()
	{
		AddMusic("Maximum Power"			, "Audio/Music/01 - Maximum Power.ogg");
		AddMusic("Final Take Off"			, "Audio/Music/07 - Final Take Off.ogg");
		AddMusic("Red Out"					, "Audio/Music/03 - Red Out.ogg");
		AddMusic("Super Stripe"				, "Audio/Music/04 - Super Stripe.ogg");
		AddMusic("After Burner"				, "Audio/Music/08 - After Burner.ogg");
		AddMusic("City 202"					, "Audio/Music/06 - City 202.ogg");

		//AddMusic("Final Take Off"					, "Audio/Music/02 - Final Take Off.wav");
		//AddMusic("After Burner"					, "Audio/Music/05 - After Burner.wav");

		AddSoundEffect("BulletFire", "Audio/BulletFire.wav");
		AddSoundEffect("FireScream", "Audio/FireScream.wav");
		AddSoundEffect("RocketFire_00", "Audio/RocketFire_00.wav");
		AddSoundEffect("RocketFire_01", "Audio/RocketFire_01.wav");
		AddSoundEffect("RocketFire_02", "Audio/RocketFire_02.wav");
		AddSoundEffect("Explosion_00", "Audio/Explosion_00.wav");
		AddSoundEffect("BurnStart", "Audio/BurnStart.wav");
		AddSoundEffect("LevelComplete", "Audio/LevelComplete.wav");
		AddSoundEffect("Bling", "Audio/Bling.wav");

		
		AddSoundEffect("CoinBack", "Audio/CoinBack.wav");
		AddSoundEffect("Pickup_Coin", "Audio/Pickup_Coin.wav");
		AddSoundEffect("MenuSelect", "Audio/MenuSelect.wav");
		AddSoundEffect("MenuSelect2", "Audio/MenuSelect2.wav");
		AddSoundEffect("Hit_00", "Audio/Hit_00.wav");
		AddSoundEffect("Hit_01", "Audio/Hit_00.wav");
		AddSoundEffect("Hit_02", "Audio/Hit_00.wav");
		AddSoundEffect("NewRocket", "Audio/NewRocket.wav");

		

		AddSoundEffect("GetReady", "Audio/GetReady.wav");
		AddSoundEffect("BeCareful", "Audio/BeCareful.wav");
		AddSoundEffect("TheEnemy", "Audio/TheEnemy.wav");

		
	}

	static void Cleanup();

	static void AddSoundEffect(const std::string& name, const std::string& path);
	static void AddMusic(const std::string& name, const std::string& path);

	static void PlaySoundEffect(const std::initializer_list<std::string>& names, float volume = 1.0f, const int loopCount = 0);
	static void PlaySoundEffect(const std::string& name, float volume = 1.0f, const int loopCount = 0);
	static void PlayMusic(const std::string& name, bool repeat = true);
	static void PlayMusic(MusicOptions musicOption, bool repeat = true, bool overWrite = false);
	static void PlayerSelectedMusic(bool repeat = true);

	static void SetGlobalVolume(float volume);
	[[nodiscard]] static float GetGlobalVolume();

	static void SetSelectedMusic(MusicOptions music);


	inline static const std::map<MusicOptions, std::string> MUSIC_MAP
	{
		{MusicOptions::MaximumPower, "Maximum Power"},
		{MusicOptions::FinalTakeOff, "Final Take Off"},
		{MusicOptions::RedOut, "Red Out"},
		{MusicOptions::SuperStripe, "Super Stripe"},
		{MusicOptions::AfterBurner, "After Burner"},
		{MusicOptions::City202, "City 202"},
	};


	inline static const float GLOBAL_SFX_VOLUME{ 0.8f };
	inline static const float GLOBAL_MUSIC_VOLUME{ 0.5f };


private:

	inline static float m_GlobalVolumeMultiplier{ 0.1f };

	inline static MusicOptions m_SelectedMusic{MusicOptions::None};
	inline static MusicOptions m_LastPlayedMusic{MusicOptions::None};

	inline static std::map<std::string, SoundEffect*> m_SoundEffectPtrMap{};
	inline static std::map<std::string, SoundStream*> m_SoundStreamPtrMap{};

};

