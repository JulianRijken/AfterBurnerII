#include "pch.h"
#include "AudioManager.h"

#include "Jul.h"
#include "SoundEffect.h"
#include "SoundStream.h"

void AudioManager::Cleanup()
{
	for (auto const& pair : m_SoundEffectPtrMap)
		delete pair.second;

	for (auto const& pair : m_SoundStreamPtrMap)
		delete pair.second;
}

void AudioManager::AddSoundEffect(const std::string& name, const std::string& path)
{
	SoundEffect* newSoundEffect{ new SoundEffect{path} };
	m_SoundEffectPtrMap.insert({ name, newSoundEffect });

	Log(TextColor::BrightWhite,BackgroundColor::Cyan) << "+ Sound  added: " << name;
}

void AudioManager::AddMusic(const std::string& name, const std::string& path)
{
	SoundStream* newSoundEffect{ new SoundStream{path} };
	m_SoundStreamPtrMap.insert({ name, newSoundEffect });

	Log(TextColor::BrightWhite, BackgroundColor::Cyan) << "+ Music  added: " << name;
}


void AudioManager::PlaySoundEffect(const std::initializer_list<std::string>& names,float volume, const int loopCount)
{
	const int randomIndex = Jul::RandomRange(0, int(names.size()) - 1);
	auto iterator = names.begin();
	std::advance(iterator, randomIndex);
	PlaySoundEffect((*iterator),volume,loopCount);
}

void AudioManager::PlaySoundEffect(const std::string& name,float volume, const int loopCount)
{
	if (not m_SoundEffectPtrMap.contains(name))
	{
		Log(LogType::Warning) << "Audio file does not exist: " << name;
		return;
	}

	volume = Jul::Clamp01(volume);

	SoundEffect* soundEffect{ m_SoundEffectPtrMap[name] };
	soundEffect->SetVolume(static_cast<int>(volume * GLOBAL_SFX_VOLUME * m_GlobalVolumeMultiplier * 128));
	soundEffect->Play(loopCount);
}

void AudioManager::PlayMusic(const std::string& name, bool repeat)
{
	if (not m_SoundStreamPtrMap.contains(name))
	{
		Log(LogType::Warning) << "Audio file does not exist: " << name;
		return;
	}

	SoundStream::SetVolume(static_cast<int>(GLOBAL_MUSIC_VOLUME * m_GlobalVolumeMultiplier * 128));

	const SoundStream* musicClip{ m_SoundStreamPtrMap[name] };
	musicClip->Play(repeat);
}

void AudioManager::PlayMusic(MusicOptions musicOption, bool repeat, bool overWrite)
{
	if (!overWrite)
	{
		if (m_LastPlayedMusic == musicOption)
			return;
	}

	m_LastPlayedMusic = musicOption;
	

	if(musicOption == MusicOptions::None)
	{
		SoundStream::SetVolume(0);
		return;
	}

	PlayMusic(MUSIC_MAP.at(musicOption), repeat);
}

void AudioManager::PlayerSelectedMusic(bool repeat)
{
	PlayMusic(m_SelectedMusic, repeat);
}

void AudioManager::SetGlobalVolume(float volume)
{
	m_GlobalVolumeMultiplier = volume;
	SoundStream::SetVolume(static_cast<int>(GLOBAL_MUSIC_VOLUME * m_GlobalVolumeMultiplier * 128));
}

float AudioManager::GetGlobalVolume()
{
	return  m_GlobalVolumeMultiplier;
}

void AudioManager::SetSelectedMusic(MusicOptions music)
{
	m_SelectedMusic = music;
}

